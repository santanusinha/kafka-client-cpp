#ifndef KAFKA_COMMAND_H
#define KAFKA_COMMAND_H

#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio/buffer.hpp>

#include "buffer.h"
#include "hostdetails.h"

namespace Kafka {

class Command: public boost::noncopyable {
    public:
        typedef boost::shared_ptr<Command> Pointer;
        typedef boost::shared_ptr<const Command> ConstPointer;

        virtual
        ~Command();

        virtual void
        notify_finish(const ConstBufferPtr &buffer) = 0;

        virtual BufferPtr 
        get_request() = 0;

        virtual int32_t
        get_partition_id() = 0;

    protected:
        Command();

};

typedef Command::Pointer CommandPtr;
typedef Command::ConstPointer ConstCommandPtr;

} //namespace Kafka

#endif //KAFKA_COMMAND_H
