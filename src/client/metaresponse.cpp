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

Metadata::Metadata()
    :m_num_partitions(),
    m_brokers(),
    m_partitions() {
}

} //namespace Kafka
