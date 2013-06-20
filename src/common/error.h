#ifndef KAFKA_ERROR_H
#define KAFKA_ERROR_H

#include <boost/exception/all.hpp>
#include <boost/tuple/tuple_io.hpp>

namespace Kafka {

typedef boost::tuple<boost::errinfo_api_function,
                    boost::errinfo_errno> SysCallFailureInfo;

typedef boost::tuple<boost::errinfo_api_function,
                    boost::errinfo_file_name,
                    boost::errinfo_at_line> CallsiteInfo;

struct KafkaError: virtual boost::exception {};

} //namespace Kafka

#endif //KAFKA_ERROR_H
