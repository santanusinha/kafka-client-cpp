#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <boost/crc.hpp>
#include <boost/make_shared.hpp>

#include "commandtype.h"
#include "commandexecutor.h"
#include "messages.h"
#include "producecommand.h"

namespace {

struct InternalData {
    Kafka::ProduceResponsePtr m_response;
    boost::mutex m_mutex;
    boost::condition_variable m_condition;

    InternalData()
        :m_response(),
        m_mutex(),
        m_condition() {
    }

};

struct FinishHandler {
    boost::shared_ptr<InternalData> m_data_ptr;
    FinishHandler( const boost::shared_ptr<InternalData> &data_ptr )
        :m_data_ptr( data_ptr ) {
    }

    void
    handle_command_finish( const Kafka::ProduceResponsePtr &response ) {
        {
            boost::mutex::scoped_lock lock(m_data_ptr->m_mutex);
            m_data_ptr->m_response = response;
        }
        m_data_ptr->m_condition.notify_all();
    }
};

}

namespace Kafka {

ProduceCommand::Pointer
ProduceCommand::create() {
    return Pointer( new ProduceCommand );
}
    

ProduceCommand::~ProduceCommand() {
}

void
ProduceCommand::notify_finish(const ConstBufferPtr &buffer) {
    MessageProduceResponse response;
    buffer->read(response);
    std::map<std::string, TopicMessageResponseList> message_responses;    
    for(std::vector<MessageTopicResponse>::iterator topic = response.m_topics.begin();
                    topic != response.m_topics.end(); ++topic ) {
        TopicMessageResponseList topic_messages;
        for(std::vector<MessagePartitionInfoResponse>::iterator msg = (*topic).m_message_info.begin(); msg != (*topic).m_message_info.end(); ++msg ) {
            if( 0 != (*msg).m_error_code) {
                topic_messages.push_back(TopicMessageResponsePtr());
                continue;
            }
            TopicMessageResponsePtr topic_message
                                        = boost::make_shared<TopicMessageResponse>();
            topic_message->m_partition = (*msg).m_partition; 
            topic_message->m_offset = (*msg).m_offset;
            topic_messages.push_back(topic_message);
        }
        message_responses.insert(std::make_pair((*topic).m_topic, topic_messages));
    }
    ProduceResponsePtr produce_response = boost::make_shared<ProduceResponse>();
    produce_response->m_message_responses.swap(message_responses);
    m_completion_handler(produce_response);
}

BufferPtr
ProduceCommand::get_request() {
    ProduceRequest request;
    request.m_header.m_api =  PRODUCE;
    request.m_header.m_apiversion = 0;
    request.m_header.m_correlation_id = 55;
    request.m_required_acks = m_ack_type;
    request.m_timeout = -1; //CHECK
    
    ProducerTopicMessagesRequest topicMessagesRequest;
    topicMessagesRequest.m_partition = m_partition;
    int32_t msg_set_size = 0;
    boost::crc_32_type result;
    static const size_t MESSAGE_MIN_SIZE     = sizeof(uint32_t)    //CRC
                                             + sizeof(int8_t)    //MAGIC
                                             + sizeof(int8_t)    //ATTRIB
                                             + sizeof(int32_t)   //KEY BUFFER SIZE 
                                             + sizeof(int32_t);  //DATA BUFFER SIZE
    static const size_t MESSAGE_START_OFFSET = sizeof(int32_t)   //SIZE ELEMENT IN BUFFER
                                             + sizeof(int64_t)   //OFFSET
                                             + sizeof(int32_t)   //MESSAGE SIZE
                                             + sizeof(uint32_t); //MESSAGE CRC
                                            
    for(std::vector<std::string>::iterator msg = m_messages.begin();
                                msg != m_messages.end(); ++msg ) {
        std::string user_message = *msg;
        BufferPtr tmpbuffer = Buffer::create_for_write();
        MessageRequest message;
        message.m_offset = 0;
        message.m_message_size = user_message.length() + MESSAGE_MIN_SIZE;
        message.m_crc = 0;
        message.m_magic_byte = 0;
        message.m_attributes = 0;
        //std::cout<<"Sending one message: " <<user_message<<std::endl;
        message.m_data.insert(message.m_data.end(),
                    user_message.c_str(), user_message.c_str() + user_message.length());
        tmpbuffer->write(message);
        void *data = reinterpret_cast<char *>(tmpbuffer->data()) + MESSAGE_START_OFFSET;
        result.process_bytes(data, tmpbuffer->size() - MESSAGE_START_OFFSET);
        message.m_crc = result.checksum();
        //std::cout<<"DS: "<<tmpbuffer->data()
        //        <<" D: "<<data
        //        <<" C: "<<result.checksum()
        //        <<" AS: "<< tmpbuffer->size()
        //        <<" S: "<<tmpbuffer->size() - MESSAGE_START_OFFSET<<std::endl;
        msg_set_size += tmpbuffer->size() - 4;//Because buffer always prepends
                                              //space for a size element
        topicMessagesRequest.m_message_set.push_back(message);
    }
    topicMessagesRequest.m_message_set_size = msg_set_size;

    ProduceTopicRequest topicRequest;
    topicRequest.m_topic_name = m_topic;
    topicRequest.m_messages.push_back(topicMessagesRequest);

    request.m_topics.push_back(topicRequest);

    BufferPtr buffer = Buffer::create_for_write();
    buffer->write(request)
          ->finalize_header();
    return buffer;
}

PartitionInfoPtr
ProduceCommand::get_partition() {
    return boost::make_shared<PartitionInfo>(m_topic, m_partition);
}

ProduceCommand::Pointer
ProduceCommand::ack_type(int32_t in_ack_type) {
    m_ack_type = in_ack_type;
    return shared_from_this();
}

ProduceCommand::Pointer
ProduceCommand::topic(const std::string &in_topic) {
    m_topic = in_topic;
    return shared_from_this();
}

ProduceCommand::Pointer
ProduceCommand::partition(int32_t in_partition) {
    m_partition = in_partition;
    return shared_from_this();
}

ProduceCommand::Pointer
ProduceCommand::message(const std::string &in_message) {
    m_messages.push_back( in_message );
    return shared_from_this();
}

ProduceCommand::Pointer
ProduceCommand::completion_handler(const ProduceCommand::CompletionFunction &handler) {
    m_completion_handler = handler;
    return shared_from_this();
}


ProduceCommand::Pointer
ProduceCommand::run(
            const boost::shared_ptr<CommandExecutor> &executor ) throw(KafkaError) {
    executor->submit(shared_from_this());
    return shared_from_this();
}

ProduceCommand::Pointer
ProduceCommand::run(ProduceResponsePtr &result,
    const boost::shared_ptr<CommandExecutor> &executor ) throw(KafkaError) {
    boost::shared_ptr<InternalData> data = boost::make_shared<InternalData>();
    m_completion_handler = boost::bind( &FinishHandler::handle_command_finish,
                    boost::shared_ptr<FinishHandler>( new FinishHandler( data )), _1);
    executor->submit(shared_from_this());
    {
        boost::mutex::scoped_lock lock(data->m_mutex);
        while( !data->m_response ) {
            data->m_condition.wait(lock);
        }
        result = data->m_response;
    }
    return shared_from_this();
}

ProduceCommand::ProduceCommand()
    :m_ack_type(),
    m_topic(),
    m_partition(),
    m_messages(),
    m_completion_handler() {
}


} //namespace Kafka
