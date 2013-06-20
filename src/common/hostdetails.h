#ifndef KAFKA_HOSTDETAILS_H
#define KAFKA_HOSTDETAILS_H

#include <string>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

namespace Kafka {

class HostDetails {
    public:
        typedef boost::shared_ptr<HostDetails> Pointer;
        typedef boost::shared_ptr<const HostDetails> ConstPointer;

        HostDetails();

        HostDetails(const std::string &in_host, int32_t in_port);

        HostDetails( const HostDetails &rhs );

        HostDetails &
        operator =( const HostDetails &rhs );

        inline std::string
        host() const;

        void
        host( const std::string &in_host );

        inline int32_t
        port() const;

        void
        port(int32_t in_port);

    private:
        std::string m_host;
        int32_t m_port;
};

typedef HostDetails::Pointer HostDetailsPtr;
typedef HostDetails::ConstPointer ConstHostDetailsPtr;

inline std::string
HostDetails::host() const {
    return m_host;
}

inline int32_t
HostDetails::port() const {
    return m_port;
}

} //namespace Kafka

#endif //KAFKA_HOSTDETAILS_H
