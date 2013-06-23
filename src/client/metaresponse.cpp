#include "metaresponse.h"

namespace Kafka {

Broker::Broker()
    :m_broker_id(),
    m_host() {
}

Partition::Partition()
    :m_node_id(),
    m_leader() {
}

Topic::Topic()
    :m_name(),
    m_num_partitions(),
    m_partitions() {
}

Metadata::Metadata()
    :m_brokers(),
    m_topics() {
}

} //namespace Kafka
