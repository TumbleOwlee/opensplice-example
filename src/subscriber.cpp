#include <iostream>
#include <thread>
#include <chrono>

#include "HelloWorldData_DCPS.hpp"
#include "HelloWorldDataSplDcps.h"

int main(int argc, char *argv[])
{
    int result = 0;
    try
    {
        dds::domain::DomainParticipant dp(org::opensplice::domain::default_id());
        dds::topic::qos::TopicQos topicQos = dp.default_topic_qos()
                                                    << dds::core::policy::Durability::Transient()
                                                    << dds::core::policy::Reliability::Reliable();
        dds::topic::Topic<HelloWorldData::Msg> topic(dp, "HelloWorldData_Msg", topicQos);

        std::string name = "HelloWorld example";
        dds::sub::qos::SubscriberQos subQos = dp.default_subscriber_qos() << dds::core::policy::Partition(name);
        dds::sub::Subscriber sub(dp, subQos);

        dds::sub::qos::DataReaderQos drqos = topic.qos();

        dds::sub::DataReader<HelloWorldData::Msg> dr(sub, topic, drqos);

        bool sampleReceived = false;
        int count = 0;
        do
        {
            dds::sub::LoanedSamples<HelloWorldData::Msg> samples = dr.take();
            for (dds::sub::LoanedSamples<HelloWorldData::Msg>::const_iterator sample = samples.begin();
                 sample < samples.end();
                 ++sample)
            {
                if (sample->info().valid())
                {
                    std::cout << "=== [Subscriber] message received ===" << std::endl;
                    sampleReceived = true;
                }
            }

            std::cout << "+++ Sleep 1000ms +++" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            ++count;
        }
        while (!sampleReceived && count < 600);

        if (!sampleReceived)
        {
            std::cerr << "ERROR: Waited for 60 seconds but no sample received" << std::endl;
            result = 1;
        }
    }
    catch (const dds::core::Exception& e)
    {
        std::cerr << "ERROR: Exception: " << e.what() << std::endl;
        result = 1;
    }
    return result;
}