#ifndef KAFKA_SERIALIZABLE_H
#define KAFKA_SERIALIZABLE_H

#include <boost/noncopyable.hpp>

#include "buffer.h"

namespace Kafka {

class Serializable: public boost::noncopyable {
    public:
        virtual void
        save(Buffer &buffer) = 0;

    protected:
        Serializable();
};

} //namespace Kafka

#endif //KAFKA_SERIALIZABLE_H
