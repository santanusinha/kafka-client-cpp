#ifndef KAFKA_BUFFER_H
#define KAFKA_BUFFER_H

#include <cstddef>
#include <string>

#include <boost/cstdint.hpp>

#include "error.h"

namespace Kafka {

class Buffer {
    public:
        Buffer();

        Buffer(void *data, size_t size);

        Buffer(const Buffer &rhs);

        Buffer &
        operator =(const Buffer &rhs);

        ~Buffer() throw();

        void
        reset() throw();

        void
        swap(Buffer &rhs) throw();

        Buffer &
        write(int64_t in_value) throw(KafkaError);

        Buffer &
        write(uint64_t in_value) throw(KafkaError);

        Buffer &
        write(int32_t in_value) throw(KafkaError);

        Buffer &
        write(uint32_t in_value) throw(KafkaError);

        Buffer &
        write(int16_t in_value) throw(KafkaError);

        Buffer &
        write(uint16_t in_value) throw(KafkaError);

        Buffer &
        write(const std::string &in_value) throw(KafkaError);

        Buffer &
        write( const void *in_data, size_t in_data_size) throw(KafkaError);

        Buffer &
        build() throw(KafkaError);

        const Buffer &
        read(int64_t &out_value) const throw(KafkaError);

        const Buffer &
        read(uint64_t &out_value) const throw(KafkaError);

        const Buffer &
        read(int32_t &out_value) const throw(KafkaError);

        const Buffer &
        read(uint32_t &out_value) const throw(KafkaError);

        const Buffer &
        read(int16_t &out_value) const throw(KafkaError);

        const Buffer &
        read(uint16_t &out_value) const throw(KafkaError);

        const Buffer &
        read(std::string &out_value) const throw(KafkaError);

        inline void *
        data() const;

        inline size_t
        size() const;

    private:
        void
        write_16(void *mem) throw(KafkaError);

        void
        write_32(void *mem) throw(KafkaError);

        void
        write_64(void *mem) throw(KafkaError);

        void *
        get(size_t size) throw(KafkaError);

        const void *
        read_16() const throw(KafkaError);

        const void *
        read_32() const throw(KafkaError);

        const void *
        read_64() const throw(KafkaError);

        const void *
        get(size_t size) const throw(KafkaError);

        void *m_data;
        mutable size_t m_current_size;
        size_t m_actual_size;
};

inline void *
Buffer::data() const {
    return m_data;
}

inline size_t
Buffer::size() const {
    return m_current_size;
}

} //namespace Kafka


#endif //KAFKA_BUFFER_H
