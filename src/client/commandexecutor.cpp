#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <boost/make_shared.hpp>

#include "commandexecutor.h"

namespace Kafka {

CommandExecutor::CommandExecutor()
    :m_io_service(),
    m_work(new boost::asio::io_service::work(m_io_service)),
    m_active_commands(),
    m_stop(),
    m_active_commands_lock(),
    m_active_commands_cond(),
    m_kafka_sockets() {
}

CommandExecutor::~CommandExecutor() {
}

void
CommandExecutor::run() {
    boost::thread main_loop( boost::bind( &CommandExecutor::start , this ) );
    main_loop.join();
}

void
CommandExecutor::connect() {
    boost::asio::ip::tcp::endpoint endpoint(
    boost::asio::ip::address::from_string("127.0.0.1"), 9093);
    Socket kafka_socket(new boost::asio::ip::tcp::socket(m_io_service));
    m_kafka_sockets.insert(std::make_pair(0, kafka_socket));
    kafka_socket->connect(endpoint);
}

void
CommandExecutor::submit(const CommandPtr &command) throw(KafkaError) {
    {
        boost::mutex::scoped_lock lock(m_active_commands_lock);
        if(m_active_commands.size() > m_max_queue_size) {
            //TODO::THROW ERROR
        }
        m_active_commands.push(command);
        std::cout<<"Command pushed"<<std::endl;
    }
    m_active_commands_cond.notify_one();
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
        m_active_commands.pop();
        std::cout<<"Command received"<<std::endl;
        std::map<int32_t, Socket>::iterator it
                    = m_kafka_sockets.find(command->get_partition_id());
        if( it == m_kafka_sockets.end() ) {
            //TODO::THROW/LOG ERROR
            std::cout<<"Socket not found"<<std::endl;
            continue;
        }
        Socket kafka_socket = (*it).second;
        boost::asio::write( *kafka_socket,
                 boost::asio::buffer(command->get_request()->build()),
                 boost::asio::transfer_all());
        std::cout<<"Message sent"<<std::endl;
        boost::system::error_code error;
        uint32_t recv_size = 0;
        size_t len = kafka_socket->read_some(
                            boost::asio::buffer(&recv_size, sizeof(uint32_t)), error);
        recv_size = ntohl(recv_size);
        std::cout<<"Message size: <<"<<recv_size<<std::endl;
        char *read_buf = new char[recv_size]; //TODO::PERF
        len = kafka_socket->read_some(boost::asio::buffer(read_buf, recv_size), error);
        std::cout<<"Message received"<<std::endl;
        ConstBufferPtr reply_buffer
            = Buffer::create_for_read(boost::asio::buffer(read_buf, len));
        delete []read_buf;
        command->notify_finish(reply_buffer);
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
