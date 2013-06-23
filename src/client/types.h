#ifndef KAFKA_TYPES_H
#define KAFKA_TYPES_H

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

namespace Kafka {

typedef boost::shared_ptr<boost::asio::ip::tcp::socket> Socket;

} //namespace Kafka


#endif //KAFKA_TYPES_H
