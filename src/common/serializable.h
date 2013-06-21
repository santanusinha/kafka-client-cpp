#ifndef KAFKA_SERIALIZABLE_H
#define KAFKA_SERIALIZABLE_H

#include "buffer.h"

namespace Kafka {

class Serializable {
    public:
        virtual
        ~Serializable();

        virtual void
        save(const BufferPtr &buffer) const = 0;

    protected:
        Serializable();
};

} //namespace Kafka

#endif //KAFKA_SERIALIZABLE_H
