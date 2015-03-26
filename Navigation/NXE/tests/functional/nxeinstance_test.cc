#include "nxe_instance.h"
#include "navitprocessimpl.h"
#include "navitcontroller.h"
#include "navitipc.h"
#include "navitdbus.h"
#include "testutils.h"

#include <gtest/gtest.h>
#include <memory>
#include <chrono>
#include <thread>

struct NXEInstanceTest : public ::testing::Test {

    std::shared_ptr<NXE::NavitProcess> np{ new NXE::NavitProcessImpl };
    std::shared_ptr<NXE::NavitIPCInterface> nc{ new NXE::NavitDBus };
    NXE::NXEInstance instance{ np, nc };
    NXE::JSONMessage respMsg;
    std::mutex responseMutex;
    bool receivedRender {false};
    std::size_t numberOfResponses = 0;

    static void SetUpTestCase()
    {
        TestUtils::createNXEConfFile();
    }

    void SetUp() {
        nc->start();
    }

    void TearDown() {
        nc->stop();
    }

    void callback(const std::string &response) {
        {
            std::lock_guard<std::mutex> guard {responseMutex};
            nDebug() << "Callback";
            if (response.size() == 7171272) {
                receivedRender = true;
            } else {
                respMsg = NXE::JSONUtils::deserialize(response);
                nDebug() << response;
            }
            nDebug() << "Eof Callback";
        }
        numberOfResponses++;
    }

    void zoom(int factor) {
        std::string msg{ TestUtils::zoomByMessage(factor) };
        instance.HandleMessage(msg.data());
    }
};

TEST_F(NXEInstanceTest, zoomBy)
{
    instance.registerMessageCallback(std::bind(&NXEInstanceTest::callback, this, std::placeholders::_1));
    instance.Initialize();
    std::chrono::milliseconds dura( 1000 );
    std::this_thread::sleep_for(dura);
    EXPECT_NO_THROW(
        for(int i = 0 ; i < 10 ; ++i) {
            zoom(2);
        }
    );
    ASSERT_EQ(respMsg.call, "zoomBy");
    EXPECT_TRUE(respMsg.data.empty());
    std::this_thread::sleep_for(dura);
}

TEST_F(NXEInstanceTest, speechTest) {
    // by default each time we want to draw a
    // speech 'draw' will be triggered
    instance.Initialize();
    std::chrono::milliseconds dura( 1000 );
    std::this_thread::sleep_for(dura);
    EXPECT_NO_THROW(
        for(int i = 0 ; i < 10 ; ++i) {
            zoom(2);
        }
    );
}

TEST_F(NXEInstanceTest, zoomOut)
{
    instance.registerMessageCallback(std::bind(&NXEInstanceTest::callback, this, std::placeholders::_1));
    EXPECT_NO_THROW(instance.Initialize());
    std::chrono::milliseconds dura( 1000 );
    std::this_thread::sleep_for(dura);
    EXPECT_NO_THROW(
        for(int i = 0 ; i < 10 ; ++i)
            zoom(-2);
    );
    ASSERT_EQ(respMsg.call, "zoomBy");
    EXPECT_TRUE(respMsg.data.empty());
    std::this_thread::sleep_for(dura);
}

TEST_F(NXEInstanceTest, zoom)
{
    std::string msg{ TestUtils::zoomMessage() };
    instance.registerMessageCallback(std::bind(&NXEInstanceTest::callback, this, std::placeholders::_1));
    EXPECT_NO_THROW(instance.Initialize());
    std::chrono::milliseconds dura( 100 );
    std::this_thread::sleep_for(dura);
    EXPECT_NO_THROW(
        instance.HandleMessage(msg.data());
    );

    EXPECT_FALSE(respMsg.data.empty());
}

TEST_F(NXEInstanceTest, renderBenchmarkTest)
{
    std::string msg{ TestUtils::renderMessage() };
    instance.registerMessageCallback(std::bind(&NXEInstanceTest::callback, this, std::placeholders::_1));
    EXPECT_NO_THROW(instance.Initialize());
    std::chrono::milliseconds dura( 100 );
    std::this_thread::sleep_for(dura);
    EXPECT_NO_THROW(
        for(int i = 0 ; i < 100 ; ++i) {
            instance.HandleMessage(msg.data());
        }
    );
    std::vector<double> mes = instance.renderMeasurements();
    double mean = std::accumulate(mes.begin(), mes.end(), 0.0)/mes.size();
    perfLog("render") << " mean = " << mean;
    // should be 100
    EXPECT_LT(mean, 200.0);
    EXPECT_TRUE(receivedRender);
}

TEST_F(NXEInstanceTest, DISABLED_zoomedRenderBenchmarkTest)
{
    std::string msg{ TestUtils::renderMessage() };
    instance.registerMessageCallback(std::bind(&NXEInstanceTest::callback, this, std::placeholders::_1));
    EXPECT_NO_THROW(instance.Initialize());
    std::chrono::milliseconds dura( 100 );
    std::this_thread::sleep_for(dura);
    zoom(8);
    EXPECT_NO_THROW(
        for(int i = 0 ; i < 20; ++i) {
            instance.HandleMessage(msg.data());
        }
    );
    EXPECT_TRUE(receivedRender);
}

TEST_F(NXEInstanceTest, DISABLED_zoomInAndOut)
{
    std::string msg1{ TestUtils::zoomByMessage(2) };
    std::string msg2{ TestUtils::zoomByMessage(-2) };

    instance.registerMessageCallback(std::bind(&NXEInstanceTest::callback, this, std::placeholders::_1));
    EXPECT_NO_THROW(instance.Initialize());
    std::chrono::milliseconds dura( 100 );
    std::this_thread::sleep_for(dura);
    EXPECT_NO_THROW(
        for(int i = 0 ; i < 10 ; ++i) {
            instance.HandleMessage(msg1.data());
            instance.HandleMessage(msg2.data());
        }
    );

    EXPECT_TRUE(respMsg.data.empty());
    EXPECT_EQ(respMsg.call, "zoomBy");
}

TEST_F(NXEInstanceTest, renderOneFrame)
{
    std::string msg{ TestUtils::renderMessage() };
    instance.registerMessageCallback(std::bind(&NXEInstanceTest::callback, this, std::placeholders::_1));
    EXPECT_NO_THROW(instance.Initialize());
    std::chrono::milliseconds dura( 100 );
    std::this_thread::sleep_for(dura);
    EXPECT_NO_THROW(
        instance.HandleMessage(msg.data());
    );
    std::vector<double> mes = instance.renderMeasurements();
    double mean = std::accumulate(mes.begin(), mes.end(), 0.0)/mes.size();
    perfLog("render") << " mean = " << mean;
    EXPECT_LT(mean, 200.0);
    // Message cannot be properly parsed!
    EXPECT_EQ(numberOfResponses,2);
}

TEST_F(NXEInstanceTest, moveByMessage)
{
    const std::string msg{ TestUtils::moveByMessage(10,10) };
    instance.registerMessageCallback(std::bind(&NXEInstanceTest::callback, this, std::placeholders::_1));
    EXPECT_NO_THROW(instance.Initialize());
    std::chrono::milliseconds dura( 100 );
    std::this_thread::sleep_for(dura);
    EXPECT_NO_THROW(
        instance.HandleMessage(msg.data());
    );
}
