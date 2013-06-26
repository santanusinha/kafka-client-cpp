#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <set>
#include <iostream>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>


#include "connectionpool.h"

namespace {

struct compare {
bool
operator ()(const Kafka::PartitionInfoPtr &lhs, const Kafka::PartitionInfoPtr &rhs) {
    return *lhs < *rhs;
}
};

typedef std::map<Kafka::PartitionInfoPtr, Kafka::Socket, compare> SocketMap;

}

namespace Kafka {

struct ConnectionPool::InternalData {
    boost::mutex m_sockets_mutex;
    SocketMap m_sockets;
    HostList m_broker_hosts;

    InternalData()
        :m_sockets_mutex(),
        m_sockets(),
        m_broker_hosts() {
    }
};

ConnectionPool::Pointer
ConnectionPool::create(boost::asio::io_service &io_service) {
    return Pointer( new ConnectionPool(io_service));
}

void
ConnectionPool::setup_broker_connection(const HostList &brokers) throw(KafkaError) {
    boost::mutex::scoped_lock lock(m_data->m_sockets_mutex);
    for(HostList::const_iterator host = brokers.begin(); host != brokers.end(); ++host ){
        std::ostringstream sout;
        sout<<(*host)->port();
        boost::asio::ip::tcp::resolver::query query(
                                (*host)->host(), sout.str().c_str(),
                                boost::asio::ip::resolver_query_base::numeric_service);
        boost::asio::ip::tcp::resolver resolver(m_io_service);
        boost::system::error_code error = boost::asio::error::host_not_found;
        for(boost::asio::ip::tcp::resolver::iterator i = resolver.resolve(query);
                            i != boost::asio::ip::tcp::resolver::iterator(); ++i) {
            boost::asio::ip::tcp::endpoint end = *i;
            m_meta_channel = Socket( new boost::asio::ip::tcp::socket(m_io_service) );
            m_meta_channel->connect(end, error);
            if(!error) {
                std::cout<<"BROKER CONNECTION INITIALIZED"<<std::endl;
                break; //Connection established
            }
        }
        if(error) {
            //TODO::Error?
        }
    }
}

ConnectionPool::~ConnectionPool() {
}

Socket
ConnectionPool::meta_channel() const throw(KafkaError) {
    return m_meta_channel;
}

Socket
ConnectionPool::leader(
        const PartitionInfoPtr &partition ) const throw(KafkaError) {
    boost::mutex::scoped_lock lock(m_data->m_sockets_mutex);
    SocketMap::iterator socket_info = m_data->m_sockets.find(partition);
    if( socket_info != m_data->m_sockets.end() ) {
        return (*socket_info).second;
    }
    return Socket();
}

std::vector<std::string>
ConnectionPool::topics() const {
    std::set<std::string> topics;
    for(SocketMap::const_iterator partition_info = m_data->m_sockets.begin();
                    partition_info != m_data->m_sockets.end(); ++partition_info ) {
        topics.insert((*partition_info).first->m_topic);
    }
    std::vector<std::string> topiclist;
    topiclist.insert(topiclist.end(), topics.begin(), topics.end());
    return topiclist;
}

bool
ConnectionPool::setup(const MetadataPtr &metadata) throw(KafkaError) {
    boost::mutex::scoped_lock lock(m_data->m_sockets_mutex);
    std::map<int32_t, Socket> broker_sockets;
    boost::asio::ip::tcp::resolver resolver(m_io_service);
    for(std::vector<BrokerPtr>::iterator broker = metadata->m_brokers.begin();
                broker != metadata->m_brokers.end(); broker++ ) {
        Socket socket( new boost::asio::ip::tcp::socket( m_io_service ));
        HostDetailsPtr host = (*broker)->m_host;
        std::ostringstream sout;
        sout<<host->port();
        boost::asio::ip::tcp::resolver::query query(
                                host->host(), sout.str().c_str(),
                                boost::asio::ip::resolver_query_base::numeric_service);
        boost::system::error_code error = boost::asio::error::host_not_found;
        for(boost::asio::ip::tcp::resolver::iterator i = resolver.resolve(query);
                            i != boost::asio::ip::tcp::resolver::iterator(); ++i) {
            boost::asio::ip::tcp::endpoint end = *i;
            socket->connect(end, error);
            if(!error) {
                broker_sockets.insert(std::make_pair((*broker)->m_broker_id, socket));
                break; //Connection established
            }
        }
        if(error) {
            //TODO::Error?
        }
    }
    SocketMap socket_map;
    for(std::vector<TopicPtr>::iterator topic = metadata->m_topics.begin();
                                        topic != metadata->m_topics.end(); ++topic ) {
        TopicPtr curr_topic = *topic;
        for(std::vector<PartitionPtr>::iterator partition
                                                = curr_topic->m_partitions.begin();
                    partition != curr_topic->m_partitions.end(); ++partition ) {
            std::map<int32_t, Socket>::iterator it = broker_sockets.find(
                                                (*partition)->m_leader->m_broker_id);
            if( it == broker_sockets.end() ) {
                return false; //Probably undergoing leader election
            }
            Socket broker_socket = (*it).second;
            socket_map.insert(std::make_pair(
                    boost::make_shared<PartitionInfo>(
                        curr_topic->m_name, (*partition)->m_node_id),
                    broker_socket));
        }
    }
    m_data->m_sockets.swap(socket_map);
    return true;
}

ConnectionPool::ConnectionPool(boost::asio::io_service &io_service)
    :m_io_service(io_service),
    m_meta_channel(),
    m_data( new InternalData ) {
}

} //namespace Kafka

