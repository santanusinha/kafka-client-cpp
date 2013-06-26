#include <iostream>
#include <boost/make_shared.hpp>

#include "commandexecutor.h"
#include "producecommand.h"

using namespace Kafka;




struct Worker {
    boost::shared_ptr<CommandExecutor> m_executor;
    boost::mutex m_lock;
    bool m_stop;
    int32_t m_partiton;
    int64_t m_count;

    Worker(const boost::shared_ptr<CommandExecutor> &executor)
        :m_executor(executor),
        m_lock(),
        m_stop(),
        m_partiton(),
        m_count() {
    }

    void run() {
        ProduceResponsePtr response;
        while( true ) {
            bool to_stop = false;
            {
                boost::mutex::scoped_lock l(m_lock);
                to_stop = m_stop;
            }
            if( to_stop ) {
                break;
            }
            ProduceCommand::create()->topic("precogtest")
                            ->partition(m_partiton)
                            ->ack_type(-1)
                            ->message("Test message")
                            //->message("Test message 2")
                            ->completion_handler(
                                boost::bind(&Worker::topic_message_response_handler, this, _1))
                            ->run(m_executor);
        }
    }

    void
    topic_message_response_handler(const ProduceResponsePtr &response) {
        //for(std::map<std::string, TopicMessageResponseList>::iterator topic
        //        = response->m_message_responses.begin();
        //        topic != response->m_message_responses.end(); ++topic) {
        //    std::cout<<"Topic: "<<(*topic).first<<std::endl;
        //    for(TopicMessageResponseList::iterator partition = (*topic).second.begin();
        //                partition != (*topic).second.end(); ++partition ) {
        //        std::cout<<"P: "<<(*partition)->m_partition
        //                <<" O: "<<(*partition)->m_offset<<std::endl;
        //    }
        //}
        m_count++;
    }
    void
    stop() {
        boost::mutex::scoped_lock l(m_lock);
        m_stop = true;;
    }
};

int main() {
    boost::shared_ptr<CommandExecutor> executor = boost::make_shared<CommandExecutor>();
    boost::thread main_loop( boost::bind( &CommandExecutor::start , executor ) );
    HostList broker_list;
    broker_list.push_back( boost::make_shared<HostDetails>( "127.0.0.1", 9091 ) );
    broker_list.push_back( boost::make_shared<HostDetails>( "127.0.0.1", 9092 ) );
    broker_list.push_back( boost::make_shared<HostDetails>( "127.0.0.1", 9093 ) );
    executor->connect(broker_list);
    boost::shared_ptr<Worker> worker = boost::make_shared<Worker>(executor);
    worker->m_partiton = 0;
    boost::shared_ptr<Worker> worker1 = boost::make_shared<Worker>(executor);
    worker->m_partiton = 1;
    boost::thread workerthread(boost::bind( &Worker::run, worker ));
    boost::thread workerthread1(boost::bind( &Worker::run, worker1 ));
    sleep(8);
    worker->stop();
    worker1->stop();
    workerthread.join();
    workerthread1.join();
    executor->stop();
    main_loop.join();
    
    //std::cout<<"Message count: "<<worker->m_count/8<<std::endl;
    std::cout<<"Message count: "<<(worker->m_count + worker1->m_count)/8<<std::endl;
    return 0;
}
