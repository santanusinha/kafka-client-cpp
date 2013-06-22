#ifndef KAFKA_COMMANDTYPE_H
#define KAFKA_COMMANDTYPE_H

namespace Kafka {

enum CommandType {
    PRODUCE = 0,
    FETCH,
    OFFSET,
    METADATA,
    LEADER_AND_ISR,
    STOP_REPLICA,
    OFFSET_COMMIT,
    OFFSET_FETCH
};

} //namespace Kafka

#endif //KAFKA_COMMANDTYPE_H
