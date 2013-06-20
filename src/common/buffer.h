#ifndef KAFKA_BUFFER_H
#define KAFKA_BUFFER_H

#include <cstddef>
#include <string>

#include <boost/cstdint.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "error.h"

namespace Kafka {

class Buffer: public boost::noncopyable, public boost::enable_shared_from_this<Buffer> {
    public:
        typedef boost::shared_ptr<Buffer> Pointer;
        typedef boost::shared_ptr<const Buffer> ConstPointer;

        static Pointer
        create_for_write();

        static ConstPointer
        create_for_read(const boost::asio::const_buffer &buffer);

        ~Buffer() throw();

        void
        reset() throw();

        Pointer
        write(int64_t in_value) throw(KafkaError);

        Pointer
        write(uint64_t in_value) throw(KafkaError);

        Pointer
        write(int32_t in_value) throw(KafkaError);

        Pointer
        write(uint32_t in_value) throw(KafkaError);

        Pointer
        write(int16_t in_value) throw(KafkaError);

        Pointer
        write(uint16_t in_value) throw(KafkaError);

        Pointer
        write(const std::string &in_value) throw(KafkaError);

        Pointer
        write( const void *in_data, size_t in_data_size) throw(KafkaError);

        Pointer
        finalize_header();

        boost::asio::const_buffer
        build() throw(KafkaError);

        ConstPointer
        read(int64_t &out_value) const throw(KafkaError);

        ConstPointer
        read(uint64_t &out_value) const throw(KafkaError);

        ConstPointer
        read(int32_t &out_value) const throw(KafkaError);

        ConstPointer
        read(uint32_t &out_value) const throw(KafkaError);

        ConstPointer
        read(int16_t &out_value) const throw(KafkaError);

        ConstPointer
        read(uint16_t &out_value) const throw(KafkaError);

        ConstPointer
        read(std::string &out_value) const throw(KafkaError);

        inline void *
        data() const;

        inline size_t
        size() const;

    private:
        Buffer();

        Buffer(const boost::asio::const_buffer &buffer);

        void
        swap(Buffer &rhs) throw();

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

typedef Buffer::Pointer BufferPtr;
typedef Buffer::ConstPointer ConstBufferPtr;

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
