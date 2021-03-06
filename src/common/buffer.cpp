#ifndef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include <cstring>
#include <byteswap.h>
#include <boost/make_shared.hpp>

#include "buffer.h"
#include "serializable.h"
#include "deserializable.h"

namespace {

static const size_t INITIAL_BUFFER_SIZE = 128;

}

namespace Kafka {

Buffer::Pointer
Buffer::create_for_write() {
    Pointer tmp = boost::make_shared<Buffer>();
    tmp->write(static_cast<int32_t>(0)); //Setup size
    return tmp;
}

Buffer::ConstPointer
Buffer::create_for_read(const boost::asio::const_buffer &buffer) {
    return ConstPointer( new Buffer(buffer) );
}

Buffer::~Buffer() throw() {
    reset();
}

void
Buffer::reset() throw() {
    if( 0 != m_data ) {
        free(m_data);
        m_data = 0;
        m_current_size = 0;
        m_actual_size = 0;
    }
}

Buffer::Pointer
Buffer::write(int8_t in_value) throw(KafkaError) {
    write_8(&in_value);
    return shared_from_this();
}

Buffer::Pointer
Buffer::write(uint8_t in_value) throw(KafkaError) {
    write_8(&in_value);
    return shared_from_this();
}

Buffer::Pointer
Buffer::write(int64_t in_value) throw(KafkaError) {
    write_64(&in_value);
    return shared_from_this();
}

Buffer::Pointer
Buffer::write(uint64_t in_value) throw(KafkaError) {
    write_64(&in_value);
    return shared_from_this();
}

Buffer::Pointer
Buffer::write(int32_t in_value) throw(KafkaError) {
    write_32(&in_value);
    return shared_from_this();
}

Buffer::Pointer
Buffer::write(uint32_t in_value) throw(KafkaError) {
    write_32(&in_value);
    return shared_from_this();
}

Buffer::Pointer
Buffer::write(int16_t in_value) throw(KafkaError) {
    write_16(&in_value);
    return shared_from_this();
}

Buffer::Pointer
Buffer::write(uint16_t in_value) throw(KafkaError) {
    write_16(&in_value);
    return shared_from_this();
}

Buffer::Pointer
Buffer::write(const std::string &in_value) throw(KafkaError) {
    int16_t size = in_value.size();
    write(size);
    return write(in_value.c_str(), in_value.size());
}

Buffer::Pointer
Buffer::write( const void *in_data, size_t in_data_size) throw(KafkaError) {
    void *mem = get(in_data_size);
    memcpy(mem, in_data, in_data_size);
    return shared_from_this();
}

Buffer::Pointer
Buffer::write( const Serializable &object ) {
    Buffer::Pointer thisptr = shared_from_this();
    object.save(thisptr);
    return thisptr;
}

Buffer::Pointer
Buffer::finalize_header() {
	int32_t *count = reinterpret_cast<int32_t *>(m_data);
	*count = bswap_32(m_current_size - sizeof(int32_t));
    return shared_from_this();
}

boost::asio::const_buffer
Buffer::build() throw(KafkaError) {
    return boost::asio::buffer(m_data, m_current_size);
}

Buffer::ConstPointer
Buffer::read(int8_t &out_value) const throw(KafkaError) {
    out_value = *reinterpret_cast<const int8_t *>(read_8());
    return shared_from_this();
}

Buffer::ConstPointer
Buffer::read(uint8_t &out_value) const throw(KafkaError) {
    out_value = *reinterpret_cast<const uint8_t *>(read_8());
    return shared_from_this();
}

Buffer::ConstPointer
Buffer::read(int64_t &out_value) const throw(KafkaError) {
    out_value = *reinterpret_cast<const int64_t *>(read_64());
    return shared_from_this();
}

Buffer::ConstPointer
Buffer::read(uint64_t &out_value) const throw(KafkaError) {
    out_value = *reinterpret_cast<const uint64_t *>(read_64());
    return shared_from_this();
}

Buffer::ConstPointer
Buffer::read(int32_t &out_value) const throw(KafkaError) {
    out_value = *reinterpret_cast<const int32_t *>(read_32());
    return shared_from_this();
}

Buffer::ConstPointer
Buffer::read(uint32_t &out_value) const throw(KafkaError) {
    out_value = *reinterpret_cast<const uint32_t *>(read_32());
    return shared_from_this();
}

Buffer::ConstPointer
Buffer::read(int16_t &out_value) const throw(KafkaError) {
    out_value = *reinterpret_cast<const int16_t *>(read_16());
    return shared_from_this();
}

Buffer::ConstPointer
Buffer::read(uint16_t &out_value) const throw(KafkaError) {
    out_value = *reinterpret_cast<const uint16_t *>(read_16());
    return shared_from_this();
}

Buffer::ConstPointer
Buffer::read(std::string &out_value) const throw(KafkaError) {
    int16_t length;
    read(length);
    std::copy(reinterpret_cast<const char *>(m_data) + m_current_size,
                reinterpret_cast<const char *>(m_data) + m_current_size + length,
                std::back_inserter(out_value));
    m_current_size += length;
    return shared_from_this();
}

Buffer::ConstPointer
Buffer::read(Deserializable &object) const throw(KafkaError) {
    Buffer::ConstPointer thisptr = shared_from_this();
    object.read(thisptr);
    return thisptr;
}

Buffer::Buffer()
    :noncopyable(),
	boost::enable_shared_from_this<Buffer>(),
	m_data(),
    m_current_size(),
    m_actual_size() {
}

Buffer::Buffer(const boost::asio::const_buffer &buffer)
    : noncopyable(),
	boost::enable_shared_from_this<Buffer>(),
	m_data(),
    m_current_size(),
    m_actual_size() {
    const void *in_data = boost::asio::buffer_cast<const void *>(buffer);
    size_t in_data_size = boost::asio::buffer_size(buffer);
	if( 0 == in_data_size ) {
		return;
	}
	void *target = malloc(in_data_size);
	if( 0 == target) {
        throw KafkaError()
                ;//<<CallsiteInfo(__FUNCTION__,std::string(__FILE__), __LINE__);
	}
	::memcpy(target, in_data, in_data_size);
	m_data = target;
	m_current_size = 0;
	m_actual_size = in_data_size;
}

void
Buffer::swap(Buffer &out_tmp) throw() {
    if(this != &out_tmp) {
        std::swap(m_data,out_tmp.m_data);
        std::swap(m_current_size,out_tmp.m_current_size);
        std::swap(m_actual_size,out_tmp.m_actual_size);
    }
}

void
Buffer::write_8(void *mem) throw(KafkaError) {
    //No byteswap necessary for a byte
	*reinterpret_cast<uint8_t *>(get(1)) = *reinterpret_cast<uint8_t *>(mem);	
}

void
Buffer::write_16(void *mem) throw(KafkaError) {
	*reinterpret_cast<uint16_t *>(get(2)) = bswap_16(*reinterpret_cast<uint16_t *>(mem));	
}

void
Buffer::write_32(void *mem) throw(KafkaError) {
	*reinterpret_cast<uint32_t *>(get(4)) = bswap_32(*reinterpret_cast<uint32_t *>(mem));	
}

void
Buffer::write_64(void *mem) throw(KafkaError) {
	*reinterpret_cast<uint64_t *>(get(8)) = bswap_64(*reinterpret_cast<uint64_t *>(mem));	
}

void *
Buffer::get(size_t size) throw(KafkaError) {
    if( m_current_size + size >= m_actual_size ) {
        size_t target_size = (0 == m_actual_size)
								? INITIAL_BUFFER_SIZE : m_actual_size << 1;
        void *data = realloc(m_data, target_size );
        if( !data ) {
            throw KafkaError()
                ;//<<CallsiteInfo(__FUNCTION__,std::string(__FILE__), __LINE__);
        }
        m_data = data;
        m_actual_size = target_size;
    }
    void *ptr = reinterpret_cast<char *>(m_data) + m_current_size;
    m_current_size += size;
    return ptr;
}

const void *
Buffer::read_8() const throw(KafkaError) {
    //No byteswap necessary for a byte
    uint8_t *mem = const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(get(1)));
    return mem;
}

const void *
Buffer::read_16() const throw(KafkaError) {
    uint16_t *mem = const_cast<uint16_t *>(reinterpret_cast<const uint16_t *>(get(2)));
    *mem = bswap_16(*mem);
    return mem;
}

const void *
Buffer::read_32() const throw(KafkaError) {
    //const void *mem = get(4);
    //bswap_32(*reinterpret_cast<const uint32_t *>(mem));
    //return mem;
    uint32_t *mem = const_cast<uint32_t *>(reinterpret_cast<const uint32_t *>(get(4)));
    *mem = bswap_32(*mem);
    return mem;
}

const void *
Buffer::read_64() const throw(KafkaError) {
    //const void *mem = get(8);
    //bswap_64(*reinterpret_cast<const uint64_t *>(mem));
    //return mem;
    uint64_t *mem = const_cast<uint64_t *>(reinterpret_cast<const uint64_t *>(get(8)));
    *mem = bswap_64(*mem);
    return mem;
}

const void *
Buffer::get(size_t size) const throw(KafkaError) {
    if( size > 0
            && ( 0 == m_data || m_current_size + size > m_actual_size )) {
        throw KafkaError()
                ;//<<CallsiteInfo(__FUNCTION__,std::string(__FILE__), __LINE__);
    }
    void *data =  reinterpret_cast<char *>(m_data) + m_current_size;
    m_current_size += size;
    return data;
}

} //namespace Kafka


