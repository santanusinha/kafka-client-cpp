#include <iostream>

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

MessageRequest::MessageRequest()
    :m_crc(),
    m_magic_byte(),
    m_attributes(),
    m_key(),
    m_data() {
}

void
MessageRequest::save(const BufferPtr &buffer) const {
    buffer->write(m_offset)
          ->write(m_message_size)
          ->write(m_crc)
          ->write(m_magic_byte)
          ->write(m_attributes)
          ->write(static_cast<int32_t>(-1))//m_key);
          ->write(m_data);
}

ProducerTopicMessagesRequest::ProducerTopicMessagesRequest()
    :m_partition(),
    m_message_set_size(),
    m_message_set() {
}

void
ProducerTopicMessagesRequest::save(const BufferPtr &buffer) const {
    buffer->write(m_partition)
          ->write(m_message_set_size);
    //Message set does not have a header
    uint32_t i = 1;
    for(std::vector<MessageRequest>::const_iterator msg_set = m_message_set.begin();
                    msg_set != m_message_set.end(); ++msg_set ) {
        buffer->write(*msg_set);
        //std::cout<<"Writing "<<i<<" OFF: "<<buffer->size()<<std::endl;
        i++;
    }
} 

ProduceTopicRequest::ProduceTopicRequest()
    :m_topic_name(),
    m_messages() {
}

void
ProduceTopicRequest::save(const BufferPtr &buffer) const {
    buffer->write(m_topic_name)
          ->write(m_messages);
}

ProduceRequest::ProduceRequest()
    :m_header(),
    m_required_acks(),
    m_timeout(),
    m_topics() {
}

void
ProduceRequest::save(const BufferPtr &buffer) const {
    buffer->write(m_header)
          ->write(m_required_acks)
          ->write(m_timeout)
          ->write(m_topics);
}

MessagePartitionInfoResponse::MessagePartitionInfoResponse()
    :m_partition(),
    m_error_code(),
    m_offset() {
}

void
MessagePartitionInfoResponse::read(const ConstBufferPtr &buffer) {
    buffer->read(m_partition)
          ->read(m_error_code)
          ->read(m_offset);
}

MessageTopicResponse::MessageTopicResponse()
    :m_topic(),
    m_message_info() {
}

void
MessageTopicResponse::read(const ConstBufferPtr &buffer) {
    buffer->read(m_topic)
          ->read(m_message_info);
}

MessageProduceResponse::MessageProduceResponse()
    :m_topics() {
}

void
MessageProduceResponse::read(const ConstBufferPtr &buffer) {
    buffer->read(m_header)
          ->read(m_topics);
}

} //namespace Kafka
