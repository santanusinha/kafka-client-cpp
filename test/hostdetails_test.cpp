#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "buffer.h"

#define BOOST_TEST_MODULE KafkaBuffer
#include <boost/test/unit_test.hpp>
#include <boost/make_shared.hpp>

#include "hostdetails.h"

using namespace Kafka;

BOOST_AUTO_TEST_SUITE (KafkaHostDetails)

BOOST_AUTO_TEST_CASE (test_all) {
    HostDetailsPtr host = boost::make_shared<HostDetails>();
    host->host("google.com");
    host->port(8080);
    BOOST_REQUIRE(host->host() == "google.com");
    BOOST_REQUIRE(host->port() == 8080);

    HostDetailsPtr hostcopy = boost::make_shared<HostDetails>(*host);
    BOOST_REQUIRE(hostcopy->host() == "google.com");
    BOOST_REQUIRE(hostcopy->port() == 8080);
}

BOOST_AUTO_TEST_SUITE_END( )

