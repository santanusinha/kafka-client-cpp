#include "messages.h"

#if 0
namespace {

template<typename T>
struct IsPrimitive {
    enum { value = 0 };
};

template<>
struct IsPrimitive<std::string> {
    enum { value = 1 };
};

template<>
struct IsPrimitive<int16_t> {
    enum { value = 1 };
};

template<>
struct IsPrimitive<uint16_t> {
    enum { value = 1 };
};

template<>
struct IsPrimitive<int32_t> {
    enum { value = 1 };
};

template<>
struct IsPrimitive<uint32_t> {
    enum { value = 1 };
};

template<>
struct IsPrimitive<int64_t> {
    enum { value = 1 };
};

template<>
struct IsPrimitive<uint64_t> {
    enum { value = 1 };
};


template<typename T>
void
read_vector( std::vector<T> &elements, const Kafka::ConstBufferPtr &buffer ) {
    int32_t num_elements = 0;
    for( int32_t i = 0; i < num_elements; i++ ) {
        T object;
        if(IsPrimitive<T>::value) {
            buffer->read(object);
        }
        else {
            object.read(buffer);
        }
        elements.push_back(object);
    }
}

template<typename T>
void
write_vector( const std::vector<T> &elements, const Kafka::BufferPtr &buffer ) {
    int32_t num_elements = elements.size();
    for(typename std::vector<T>::const_iterator it = elements.begin();
                it != elements.end(); ++it ) {
        if( IsPrimitive<T>::value ) {
            buffer->write(*it);
        }
        else {
            (*it).save( buffer );
        }
    }
}

}
#endif

namespace Kafka {
void
RequestHeader::save( const BufferPtr &buffer ) const {
    buffer->write(m_api)
          ->write(m_apiversion)
          ->write(m_correlation_id)
          ->write(m_client);
}

void
MetadataRequest::save( const BufferPtr &buffer ) const {
    buffer->write(m_header)
          ->write(m_topics);
}

void
BrokerResponse::read(const ConstBufferPtr &buffer) {
    buffer->read(m_broker_id)
          ->read(m_hostname)
          ->read(m_port);
}

void
PartitionMetadataResponse::read(const ConstBufferPtr &buffer) {
    buffer->read(m_partition_error_code)
          ->read(m_node_id)
          ->read(m_leader)
          ->read(m_replicas)
          ->read(m_isr);
}

void
TopicMetadataResponse::read(const ConstBufferPtr &buffer) {
    buffer->read(m_topic_error_code)
          ->read(m_partitions);
}

void
MetadataResponse::read(const ConstBufferPtr &buffer) {
    buffer->read(m_brokers)
          ->read(m_topics);
}

} //namespace Kafka
