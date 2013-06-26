#ifndef KAFKA_PRODUCECOMMAND_H
#define KAFKA_PRODUCECOMMAND_H

#include <vector>
#include <boost/function.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "command.h"
#include "error.h"
#include "partitioninfo.h"
#include "produceresponse.h"

namespace Kafka { class CommandExecutor; }

namespace Kafka {

class ProduceCommand: public Command,
                      public boost::enable_shared_from_this<ProduceCommand> {
    public:
        typedef boost::function<void(const ProduceResponsePtr &)> CompletionFunction;
        typedef boost::shared_ptr<ProduceCommand> Pointer;
        typedef boost::shared_ptr<const ProduceCommand> ConstPointer;

        static Pointer
        create();

        ~ProduceCommand();

        void
        notify_finish(const ConstBufferPtr &buffer);

        BufferPtr
        get_request();

        PartitionInfoPtr
        get_partition();

        Pointer
        ack_type(int32_t in_ack_type);

        Pointer
        topic(const std::string &in_topic);

        Pointer
        partition(int32_t in_partition);

        Pointer
        message(const std::string &in_message);

        Pointer
        completion_handler(const CompletionFunction &handler);

        Pointer
        run( const boost::shared_ptr<CommandExecutor> &executor ) throw(KafkaError);

        Pointer
        run(ProduceResponsePtr &result,
            const boost::shared_ptr<CommandExecutor> &executor ) throw(KafkaError);

    private:
        ProduceCommand();

        int32_t                  m_ack_type;
        std::string              m_topic;
        int32_t                  m_partition;
        std::vector<std::string> m_messages;
        CompletionFunction       m_completion_handler;
};

} //namespace Kafka

#endif //KAFKA_PRODUCECOMMAND_H
