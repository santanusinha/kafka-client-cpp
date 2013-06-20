#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "hostdetails.h"

namespace Kafka {
HostDetails::HostDetails()
    :m_host(),
    m_port() {
}

HostDetails::HostDetails(const std::string &in_host, int32_t in_port)
    :m_host(in_host),
    m_port(in_port) {
}

HostDetails::HostDetails( const HostDetails &rhs )
    :m_host(rhs.m_host),
    m_port(rhs.m_port) {
}

HostDetails &
HostDetails::operator =( const HostDetails &rhs ) {
    if(this != &rhs) {
        m_host = rhs.m_host;
        m_port = rhs.m_port;
    }
    return *this;
}

void
HostDetails::host( const std::string &in_host ) {
    m_host = in_host;
}

void
HostDetails::port(int32_t in_port) {
    m_port = in_port;
}

} //namespace Kafka
