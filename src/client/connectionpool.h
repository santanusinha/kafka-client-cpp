#ifndef KAFKA_CONNECTIONPOOL_H
#define KAFKA_CONNECTIONPOOL_H

#include <boost/noncopyable.hpp>

#include "error.h"
#include "metaresponse.h"
#include "partitioninfo.h"
#include "types.h"

namespace Kafka {

class ConnectionPool {
    public:
        typedef boost::shared_ptr<ConnectionPool> Pointer;
        typedef boost::shared_ptr<const ConnectionPool> ConstPointer;

        static Pointer
        create(boost::asio::io_service &io_service);

        ~ConnectionPool();

        Socket
        meta_channel() const throw(KafkaError);

        Socket
        leader( const PartitionInfoPtr &partition ) const throw(KafkaError);

        std::vector<std::string>
        topics() const;

        void
        setup_broker_connection(const HostList &brokers) throw(KafkaError);

        bool
        setup(const MetadataPtr &metadata) throw(KafkaError);

    private:
        ConnectionPool(boost::asio::io_service &io_service);

        struct InternalData;
        typedef boost::shared_ptr<InternalData> InternalDataPtr;

        boost::asio::io_service &m_io_service;
        Socket m_meta_channel;
        InternalDataPtr m_data;
};

typedef ConnectionPool::Pointer ConnectionPoolPtr;
typedef ConnectionPool::ConstPointer ConstConnectionPoolPtr;

} //namespace Kafka


#endif //KAFKA_CONNECTIONPOOL_H
