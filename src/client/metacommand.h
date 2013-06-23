#ifndef KAFKA_METACOMMAND_H
#define KAFKA_METACOMMAND_H

#include <boost/function.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "command.h"
#include "error.h"
#include "metaresponse.h"

namespace Kafka { class CommandExecutor; }

namespace Kafka {

class MetaCommand: public Command, public boost::enable_shared_from_this<MetaCommand> {
    public:
        typedef boost::function<void(const MetadataPtr &)> CompletionFunction;
        typedef boost::shared_ptr<MetaCommand> Pointer;
        typedef boost::shared_ptr<const MetaCommand> ConstPointer;

        static Pointer
        create();

        MetaCommand();

        ~MetaCommand();

        void
        notify_finish(const ConstBufferPtr &buffer);

        BufferPtr 
        get_request();

        PartitionInfoPtr 
        get_partition();

        Pointer
        topic(const std::string &in_topic);

        Pointer
        completion_handler(const CompletionFunction &handler);

        Pointer
        run( const boost::shared_ptr<CommandExecutor> &executor ) throw(KafkaError);

        Pointer
        run(MetadataPtr &result, const boost::shared_ptr<CommandExecutor> &executor ) throw(KafkaError);

    private:
        std::vector<std::string> m_topics;
        CompletionFunction       m_completion_handler;
};

typedef MetaCommand::Pointer MetaCommandPtr;

} //namespace Kafka

#endif //KAFKA_METACOMMAND_H
