#ifndef KAFKA_COMMANDEXECUTOR_H
#define KAFKA_COMMANDEXECUTOR_H

#include <queue>
#include <map>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/scoped_ptr.hpp>

#include "command.h"
#include "error.h"

namespace Kafka {

class CommandExecutor {
    public:
        CommandExecutor();

        ~CommandExecutor();

        void
        run();

        void
        connect();

        void
        submit(const CommandPtr &command) throw(KafkaError);

        void
        start();

        void
        stop();

    private:
        boost::asio::io_service m_io_service;
        boost::scoped_ptr<boost::asio::io_service::work> m_work;
        typedef boost::shared_ptr<boost::asio::ip::tcp::socket> Socket;

        static const size_t              m_max_queue_size = 100; //TODO
        std::queue<CommandPtr>    m_active_commands;        
        bool                      m_stop;
        boost::mutex              m_active_commands_lock;
        boost::condition_variable m_active_commands_cond;

        std::map<int32_t, Socket> m_kafka_sockets;
};

} //namespace Kafka 

#endif //KAFKA_COMMANDEXECUTOR_H
