#ifndef KAFKA_BUFFER_H
#define KAFKA_BUFFER_H

#include <cstddef>
#include <string>

#include <boost/static_assert.hpp>
#include <boost/cstdint.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/type_traits.hpp>

#include "error.h"

namespace Kafka { class Serializable; }
namespace Kafka { class Deserializable; }

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
        write( const Serializable &object );

        template<typename T>
        inline Pointer
        write( const std::vector<T> &elements );

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

        ConstPointer
        read( Deserializable &out_value) const throw(KafkaError);

        template<typename T>
        inline ConstPointer
        read( std::vector<T> &elements ) const throw(KafkaError);

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

namespace _Impl {

template<typename T>
struct IsBufferFriendly {
    enum { serializable = boost::is_integral<T>::value || boost::is_base_of<Serializable, T>::value,
           deserializable = boost::is_integral<T>::value || boost::is_base_of<Deserializable, T>::value };
};

template<typename T>
struct IsBufferFriendly<std::vector<T> > {
    enum { serializable = IsBufferFriendly<T>::serializable,
           deserializable = IsBufferFriendly<T>::deserializable };
};

template<>
struct IsBufferFriendly<std::string> {
    enum { serializable = 1,
           deserializable = 1 };
};

} //namespace _Impl

template<typename T>
inline Buffer::Pointer
Buffer::write( const std::vector<T> &elements ) {
    BOOST_STATIC_ASSERT(_Impl::IsBufferFriendly<T>::serializable);
    //"Type is not serializable, must be primitive, string, vector or derive from Serializable")
    int32_t num_elements = elements.size();
    write(num_elements);
    for(typename std::vector<T>::const_iterator it = elements.begin();
                it != elements.end(); ++it ) {
        write(*it);
    }
    return shared_from_this();
}

template<typename T>
inline Buffer::ConstPointer
Buffer::read( std::vector<T> &elements ) const throw(KafkaError) {
    BOOST_STATIC_ASSERT(_Impl::IsBufferFriendly<T>::deserializable);
    //"Type is not serializable, must be primitive, string, vector or derive from Serializable")
    int32_t num_elements = 0;
    read(num_elements);
    for( int32_t i = 0; i < num_elements; i++ ) {
        T object;
        read(object);
        elements.push_back(object);
    }
    return shared_from_this();
}

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
