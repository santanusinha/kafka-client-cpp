#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/scoped_ptr.hpp>

#include "commandtype.h"
#include "commandexecutor.h"
#include "kafkaerrorcodes.h"
#include "messages.h"
#include "metacommand.h"

namespace {

struct InternalData {
    Kafka::MetadataPtr m_response;
    boost::mutex m_mutex;
    boost::condition_variable m_condition;

    InternalData()
        :m_response(),
        m_mutex(),
        m_condition() {
    }
};

struct FinishHandler {
    boost::shared_ptr<InternalData> m_data_ptr;
    FinishHandler( const boost::shared_ptr<InternalData> &data_ptr )
        :m_data_ptr( data_ptr ) {
    }

    void
    handle_command_finish( const Kafka::MetadataPtr &metadata ) {
        {
            boost::mutex::scoped_lock lock(m_data_ptr->m_mutex);
            m_data_ptr->m_response = metadata;
        }
        m_data_ptr->m_condition.notify_all();
    }
};

}

namespace Kafka {

MetaCommandPtr
MetaCommand::create() {
	return boost::make_shared<MetaCommand>();
}

MetaCommand::MetaCommand()
    :m_topics(),
    m_completion_handler() {
}

MetaCommand::~MetaCommand() {
}

void
MetaCommand::notify_finish(const ConstBufferPtr &buffer) {
    MetadataResponse response;
    buffer->read(response);
    std::vector<BrokerPtr> brokers;
    std::map<int32_t, BrokerPtr> broker_map;
    for(std::vector<BrokerResponse>::iterator broker = response.m_brokers.begin();
            broker != response.m_brokers.end(); ++broker ) {
        BrokerPtr b = boost::make_shared<Broker>();
        b->m_broker_id = (*broker).m_broker_id;
        b->m_host = boost::make_shared<HostDetails>(
                                        (*broker).m_hostname, (*broker).m_port);
        brokers.push_back(b);
        broker_map.insert(std::make_pair((*broker).m_broker_id, b));
    }
    MetadataPtr metadata = boost::make_shared<Metadata>();
    std::vector<TopicPtr> topics;
    for(std::vector<TopicMetadataResponse>::iterator topic = response.m_topics.begin();
                                            topic != response.m_topics.end(); topic++) {
        if( NO_ERROR == (*topic).m_topic_error_code ) {
            TopicPtr topic_meta = boost::make_shared<Topic>();
            topic_meta->m_name = (*topic).m_topic_name;
            topic_meta->m_num_partitions = (*topic).m_partitions.size();
            std::vector<PartitionPtr> partitions;
            for(std::vector<PartitionMetadataResponse>::iterator partition
                                                        = (*topic).m_partitions.begin();
                            partition != (*topic).m_partitions.end(); ++partition ) {
                PartitionPtr p = boost::make_shared<Partition>();
                p->m_node_id = (*partition).m_node_id;
                
                p->m_leader = broker_map[(*partition).m_leader];
                partitions.push_back(p);
            }
            topic_meta->m_partitions.swap(partitions);
            topics.push_back(topic_meta);
        }
        else {
            //TODO::HANDLE
        }
    }
    metadata->m_brokers.swap(brokers);
    metadata->m_topics.swap(topics);
    m_completion_handler(metadata);
}

BufferPtr 
MetaCommand::get_request() {
	MetadataRequest request;
	request.m_header.m_api = METADATA;
	request.m_header.m_apiversion = 0;
	request.m_header.m_correlation_id = 55;
	request.m_topics.insert(request.m_topics.end(), m_topics.begin(), m_topics.end());
    BufferPtr buffer = Buffer::create_for_write();
	buffer->write(request)
          ->finalize_header();
    return buffer;
}

PartitionInfoPtr
MetaCommand::get_partition() {
    return PartitionInfoPtr();
}

MetaCommand::Pointer
MetaCommand::topic(const std::string &in_topic) {
    m_topics.push_back(in_topic);
    return shared_from_this();
}

MetaCommand::Pointer
MetaCommand::completion_handler(const MetaCommand::CompletionFunction &handler) {
    m_completion_handler = handler;
    return shared_from_this();
}

MetaCommand::Pointer
MetaCommand::run( const boost::shared_ptr<CommandExecutor> &executor ) throw(KafkaError) {
    executor->submit(shared_from_this());
    return shared_from_this();
}

MetaCommand::Pointer
MetaCommand::run(MetadataPtr &result,
                const boost::shared_ptr<CommandExecutor> &executor) throw(KafkaError) {
    boost::shared_ptr<InternalData> data = boost::make_shared<InternalData>();
    m_completion_handler = boost::bind( &FinishHandler::handle_command_finish,
                    boost::shared_ptr<FinishHandler>( new FinishHandler( data )), _1);
    executor->submit(shared_from_this());
    {
        boost::mutex::scoped_lock lock(data->m_mutex);
        while( !data->m_response ) {
            data->m_condition.wait(lock);
        }
        result = data->m_response;
    }
    return shared_from_this();
}

} //namespace Kafka

