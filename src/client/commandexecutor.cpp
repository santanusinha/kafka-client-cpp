#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <boost/make_shared.hpp>

#include "commandexecutor.h"
#include "metacommand.h"
#include "types.h"

namespace Kafka {

CommandExecutor::CommandExecutor()
    :m_io_service(),
    m_work(new boost::asio::io_service::work(m_io_service)),
    m_active_commands(),
    m_stop(),
    m_active_commands_lock(),
    m_active_commands_cond(),
    m_connection_pool( ConnectionPool::create(m_io_service) ) {
}

CommandExecutor::~CommandExecutor() {
}

void
CommandExecutor::run() {
    boost::thread main_loop( boost::bind( &CommandExecutor::start , this ) );
    main_loop.join();
}

void
CommandExecutor::connect(const HostList &brokers) throw(KafkaError) {
    m_connection_pool->setup_broker_connection(brokers);
    reset();
}

void
CommandExecutor::submit(const CommandPtr &command) throw(KafkaError) {
    {
        boost::mutex::scoped_lock lock(m_active_commands_lock);
        if(m_active_commands.size() > m_max_queue_size) {
            //TODO::THROW ERROR
        }
        m_active_commands.push_back(command);
        //std::cout<<"Command pushed"<<std::endl;
    }
    m_active_commands_cond.notify_one();
}

void
CommandExecutor::reset_connections( const MetadataPtr &metadata ) {
    m_connection_pool->setup( metadata );
}

void
CommandExecutor::reset() {
    MetaCommandPtr metacommand = MetaCommand::create();
    std::vector<std::string> topics = m_connection_pool->topics();
    for_each(topics.begin(), topics.end(),
                boost::bind( &MetaCommand::topic, metacommand, _1));
    metacommand->completion_handler(
                    boost::bind( &CommandExecutor::reset_connections, this, _1));
    //TODO::TIME BASED RETRY
    m_active_commands.push_front(metacommand);
}

void
CommandExecutor::start() {
    while(true) {
        boost::mutex::scoped_lock lock(m_active_commands_lock);
        while(!m_stop && m_active_commands.empty()) {
            m_active_commands_cond.wait(lock);
        }
        if(m_stop) {
            return;
        }
        CommandPtr command = m_active_commands.front();
        m_active_commands.pop_front();
        //std::cout<<"Command received"<<std::endl;
        PartitionInfoPtr partiton = command->get_partition();
        Socket kafka_socket;
        if(!partiton) { //Command is agnostic of channel
            kafka_socket = m_connection_pool->meta_channel();
        }
        else {
            kafka_socket = m_connection_pool->leader(partiton);
            if( !kafka_socket ) {
                //TODO::LOG ERROR
                //std::cout<<"Socket not found: ("<<partiton->m_topic
                            //<<","<<partiton->m_partiton<<")"<<std::endl;
                //TODO::SEND ERROR
                continue;
            }
        }
        try {
            boost::asio::write( *kafka_socket,
                 boost::asio::buffer(command->get_request()->build()),
                 boost::asio::transfer_all());

            //std::cout<<"Message sent"<<std::endl;
            boost::system::error_code error;
            uint32_t recv_size = 0;
            size_t len = kafka_socket->read_some(
                                boost::asio::buffer(&recv_size, sizeof(uint32_t)), error);
            recv_size = ntohl(recv_size);
            //std::cout<<"Message size: <<"<<recv_size<<std::endl;
            char *read_buf = new char[recv_size]; //TODO::PERF
            len = kafka_socket->read_some(boost::asio::buffer(read_buf, recv_size), error);
            //std::cout<<"Message received"<<std::endl;
            ConstBufferPtr reply_buffer
                = Buffer::create_for_read(boost::asio::buffer(read_buf, len));
            delete []read_buf;
            command->notify_finish(reply_buffer);
        } catch(boost::system::system_error &error) {
            m_active_commands.push_front( command );
            reset();
            continue;
        }
        //LOTS OF TODOs above
        //Timeout, Error handling
    }
}

void
CommandExecutor::stop() {
    {
        boost::mutex::scoped_lock lock(m_active_commands_lock);
        m_stop = true;
    }
    m_active_commands_cond.notify_all();
}

} //namespace Kafka
