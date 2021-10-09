#include <iostream>
#include <thread>
#include <chrono>

#include "HelloWorldData_DCPS.hpp"

int main(int argc, char *argv[])
{
    int result = 0;
    try
    {
        dds::domain::DomainParticipant dp(org::opensplice::domain::default_id());

        dds::topic::qos::TopicQos topicQos
             = dp.default_topic_qos()
                << dds::core::policy::Durability::Transient()
                << dds::core::policy::Reliability::Reliable();

        dds::topic::Topic<HelloWorldData::Msg> topic(dp, "HelloWorldData_Msg", topicQos);

        std::string name = "HelloWorld example";
        dds::pub::qos::PublisherQos pubQos = dp.default_publisher_qos() << dds::core::policy::Partition(name);
        dds::pub::Publisher pub(dp, pubQos);

        dds::pub::qos::DataWriterQos dwqos = topic.qos();
        dwqos << dds::core::policy::WriterDataLifecycle::ManuallyDisposeUnregisteredInstances();

        dds::pub::DataWriter<HelloWorldData::Msg> dw(pub, topic, dwqos);

        HelloWorldData::Msg msgInstance(1, "Hello World");
        dw << msgInstance;

        std::cout << "=== [Publisher] writing a message ===" << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        std::cout << "=== [Publisher] finished ===" << std::endl;
    }
    catch (const dds::core::Exception& e)
    {
        std::cerr << "ERROR: Exception: " << e.what() << std::endl;
        result = 1;
    }
    return result;
}
