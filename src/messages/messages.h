#ifndef KAFKA_MESSAGES_H
#define KAFKA_MESSAGES_H

#include <vector>
#include <string>
#include <boost/cstdint.hpp>

#include "serializable.h"
#include "deserializable.h"
#include "buffer.h"

namespace Kafka {

struct RequestHeader: public Serializable {
    uint16_t    m_api;
    uint16_t    m_apiversion;
    uint32_t    m_correlation_id;
    std::string m_client;

    RequestHeader();

    void
    save(const BufferPtr &buffer) const;
};

struct ResponseHeader: public Deserializable {
    uint32_t m_correlation_id;

    ResponseHeader();

    void
    read(const ConstBufferPtr &buffer);
};

struct MetadataRequest: public Serializable {
    RequestHeader            m_header;
    std::vector<std::string> m_topics;

    MetadataRequest();

    void
    save(const BufferPtr &buffer) const;
};

struct BrokerResponse: public Deserializable {
    int32_t     m_broker_id;
    std::string m_hostname;
    int32_t     m_port;

    BrokerResponse();

    void
    read(const ConstBufferPtr &buffer);
};

struct PartitionMetadataResponse: public Deserializable {
    int16_t              m_partition_error_code;
    int32_t              m_node_id;
    int32_t              m_leader;
    std::vector<int32_t> m_replicas;
    std::vector<int32_t> m_isr;

    PartitionMetadataResponse();

    void
    read(const ConstBufferPtr &buffer);
};

struct TopicMetadataResponse: public Deserializable {
    int16_t                                m_topic_error_code;
    std::string                            m_topic_name;
    std::vector<PartitionMetadataResponse> m_partitions;

    TopicMetadataResponse();
    
    void
    read(const ConstBufferPtr &buffer);
};

struct MetadataResponse: public Deserializable {
    ResponseHeader m_header;
    std::vector<BrokerResponse> m_brokers;
    std::vector<TopicMetadataResponse> m_topics;

    MetadataResponse();

    void
    read(const ConstBufferPtr &buffer);
};

} //namespace Kafka

#endif // KAFKA_MESSAGES_H
