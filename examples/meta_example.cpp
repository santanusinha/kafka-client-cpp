#include <iostream>
#include <boost/make_shared.hpp>

#include "commandexecutor.h"
#include "metacommand.h"

using namespace Kafka;

int main() {
    boost::shared_ptr<CommandExecutor> executor = boost::make_shared<CommandExecutor>();
    boost::thread main_loop( boost::bind( &CommandExecutor::start , executor ) );
    executor->connect();
    MetadataPtr metadata;
    MetaCommand::create()->topic("mytopic")->run(metadata, executor);
    std::cout<<"Completed Meta request"<<std::endl;
    executor->stop();
    main_loop.join();
    return 0;
}
