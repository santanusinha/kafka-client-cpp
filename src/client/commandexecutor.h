#ifndef KAFKA_COMMANDEXECUTOR_H
#define KAFKA_COMMANDEXECUTOR_H

#include <list>
#include <map>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/scoped_ptr.hpp>

#include "command.h"
#include "connectionpool.h"
#include "error.h"
#include "hostdetails.h"

namespace Kafka {

class CommandExecutor {
    public:
        CommandExecutor();

        ~CommandExecutor();

        void
        run();

        void
        connect(const HostList &brokers) throw(KafkaError);

        void
        submit(const CommandPtr &command) throw(KafkaError);

        void
        start();

        void
        stop();

    private:
        void
        reset_connections( const MetadataPtr &metadata );

        void
        reset();

        boost::asio::io_service m_io_service;
        boost::scoped_ptr<boost::asio::io_service::work> m_work;

        static const size_t              m_max_queue_size = 100; //TODO
        std::list<CommandPtr>     m_active_commands;        
        bool                      m_stop;
        boost::mutex              m_active_commands_lock;
        boost::condition_variable m_active_commands_cond;
        ConnectionPoolPtr         m_connection_pool;
};

} //namespace Kafka 

#endif //KAFKA_COMMANDEXECUTOR_H
