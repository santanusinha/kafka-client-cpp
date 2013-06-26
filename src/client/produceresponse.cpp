#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "produceresponse.h"

namespace Kafka {

TopicMessageResponse::TopicMessageResponse()
    :m_partition(),
    m_offset() {
}

ProduceResponse::ProduceResponse()
    :m_message_responses() {
}

} //namespace Kafka
