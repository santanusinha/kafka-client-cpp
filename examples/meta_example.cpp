#include <iostream>
#include <boost/make_shared.hpp>

#include "commandexecutor.h"
#include "metacommand.h"

using namespace Kafka;

int main() {
    boost::shared_ptr<CommandExecutor> executor = boost::make_shared<CommandExecutor>();
    boost::thread main_loop( boost::bind( &CommandExecutor::start , executor ) );
    HostList broker_list;
    broker_list.push_back( boost::make_shared<HostDetails>( "127.0.0.1", 9091 ) );
    broker_list.push_back( boost::make_shared<HostDetails>( "127.0.0.1", 9092 ) );
    broker_list.push_back( boost::make_shared<HostDetails>( "127.0.0.1", 9093 ) );
    executor->connect(broker_list);
    MetadataPtr metadata;
    MetaCommand::create()->topic("mytopic")->run(metadata, executor);
    std::cout<<"Completed Meta request"<<std::endl;
    executor->stop();
    main_loop.join();
    if(metadata) {
        for(std::vector<BrokerPtr>::iterator broker = metadata->m_brokers.begin();
                    broker != metadata->m_brokers.end(); broker++ ) {
            std::cout<<"Broker: "<<(*broker)->m_broker_id<<std::endl;
        }
        for(std::vector<TopicPtr>::iterator topic = metadata->m_topics.begin();
                topic != metadata->m_topics.end(); ++topic) {
            std::cout<<"Topic: "<<(*topic)->m_name<<std::endl;
            std::cout<<"    Parts: "<<(*topic)->m_num_partitions<<std::endl;
        }
    }
    return 0;
}
