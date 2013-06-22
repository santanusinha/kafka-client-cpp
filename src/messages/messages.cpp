#include "messages.h"

namespace Kafka {

RequestHeader::RequestHeader()
    :m_api(),
    m_apiversion(),
    m_correlation_id(),
    m_client() {
}

void
RequestHeader::save( const BufferPtr &buffer ) const {
    buffer->write(m_api)
          ->write(m_apiversion)
          ->write(m_correlation_id)
          ->write(m_client);
}

ResponseHeader::ResponseHeader()
    :m_correlation_id() {
}

void
ResponseHeader::read(const ConstBufferPtr &buffer) {
    buffer->read(m_correlation_id);
}

MetadataRequest::MetadataRequest()
    :m_header(),
    m_topics() {
}

void
MetadataRequest::save( const BufferPtr &buffer ) const {
    buffer->write(m_header)
          ->write(m_topics);
}

BrokerResponse::BrokerResponse()
    :m_broker_id(),
    m_hostname(),
    m_port() {
}

void
BrokerResponse::read(const ConstBufferPtr &buffer) {
    buffer->read(m_broker_id)
          ->read(m_hostname)
          ->read(m_port);
}

PartitionMetadataResponse::PartitionMetadataResponse()
    :m_partition_error_code(),
    m_node_id(),
    m_leader(),
    m_replicas(),
    m_isr() {
}

void
PartitionMetadataResponse::read(const ConstBufferPtr &buffer) {
    buffer->read(m_partition_error_code)
          ->read(m_node_id)
          ->read(m_leader)
          ->read(m_replicas)
          ->read(m_isr);
}

TopicMetadataResponse::TopicMetadataResponse()
    :m_topic_error_code(),
    m_partitions() {
}

void
TopicMetadataResponse::read(const ConstBufferPtr &buffer) {
    buffer->read(m_topic_error_code)
          ->read(m_topic_name)
          ->read(m_partitions);
}

MetadataResponse::MetadataResponse()
    :m_header(),
    m_brokers(),
    m_topics() {
}

void
MetadataResponse::read(const ConstBufferPtr &buffer) {
    buffer->read(m_header)
          ->read(m_brokers)
          ->read(m_topics);
}

} //namespace Kafka
