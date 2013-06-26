#ifndef KAFKA_PRODUCERESPONSE_H
#define KAFKA_PRODUCERESPONSE_H

#include <map>
#include <string>
#include <vector>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

namespace Kafka {

struct TopicMessageResponse {
    int32_t m_partition;
    int64_t m_offset;

    TopicMessageResponse();
};

typedef boost::shared_ptr<TopicMessageResponse> TopicMessageResponsePtr;
typedef std::vector<TopicMessageResponsePtr> TopicMessageResponseList;

struct ProduceResponse {
    typedef boost::shared_ptr<ProduceResponse> Pointer;
    typedef boost::shared_ptr<const ProduceResponse> ConstPointer;

    std::map<std::string, TopicMessageResponseList> m_message_responses;    

    ProduceResponse();
};

typedef ProduceResponse::Pointer ProduceResponsePtr;
typedef ProduceResponse::ConstPointer ConstProduceResponsePtr;

} //namespace Kafka

#endif //KAFKA_PRODUCERESPONSE_H
