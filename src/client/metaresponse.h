#ifndef KAFKA_METARESPONSE_H
#define KAFKA_METARESPONSE_H

#include <vector>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

#include "hostdetails.h"

namespace Kafka {

struct Broker {
    int32_t m_broker_id;
    HostDetailsPtr m_host;

    Broker();
};

typedef boost::shared_ptr<Broker> BrokerPtr;

struct Partition {
    int32_t m_node_id;
    BrokerPtr m_leader;

    Partition();
};

typedef boost::shared_ptr<Partition> PartitionPtr;

struct Topic {
    std::string m_name;
    size_t m_num_partitions;
    std::vector<PartitionPtr> m_partitions; 

    Topic();
};

typedef boost::shared_ptr<Topic> TopicPtr;

struct Metadata {
    std::vector<BrokerPtr> m_brokers;
    std::vector<TopicPtr> m_topics;

    Metadata();
};

typedef boost::shared_ptr<Metadata> MetadataPtr;

} //namespace Kafka 

#endif //KAFKA_METARESPONSE_H
