#ifndef KAFKA_PARTITIONINFO_H
#define KAFKA_PARTITIONINFO_H

#include <string>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

namespace Kafka {

struct PartitionInfo {
    std::string m_topic;
    int32_t     m_partiton;

    PartitionInfo()
        :m_topic(),
        m_partiton() {
    }

    PartitionInfo(const std::string &topic,
                    int32_t partition)
        :m_topic(topic),
        m_partiton(partition) {
    }

    bool
    operator <( const PartitionInfo &rhs ) const {
        if( m_topic < rhs.m_topic ) {
            return true;
        } else {
            if( m_partiton < rhs.m_partiton ) {
                return true;
            }
        }
        return false;
    }
};

typedef boost::shared_ptr<PartitionInfo> PartitionInfoPtr;

} //namespace Kafka


#endif //KAFKA_PARTITIONINFO_H
