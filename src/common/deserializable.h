#ifndef KAFKA_DESERIALIZABLE_H
#define KAFKA_DESERIALIZABLE_H

#include <boost/noncopyable.hpp>

#include "buffer.h"

namespace Kafka {

class Deserializable {
    public:
        virtual
        ~Deserializable();

        virtual void
        read(const ConstBufferPtr &buffer) = 0;

    protected:
        Deserializable();
};

} //namespace Kafka



#endif //KAFKA_DESERIALIZABLE_H

