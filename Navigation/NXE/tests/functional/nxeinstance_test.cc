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
    bool render {false};
    bool receivedRender {false};

    static void SetUpTestCase()
    {
        TestUtils::createNXEConfFile();
    }

    void callback(const std::string &response) {
        if (render) {
            receivedRender = true;
        } else {
            respMsg = NXE::JSONUtils::deserialize(response);
        }
    }
};

TEST_F(NXEInstanceTest, zoomBy)
{
    std::string msg{ TestUtils::zoomByMessage(2) };
    instance.registerMessageCallback(std::bind(&NXEInstanceTest::callback, this, std::placeholders::_1));
    EXPECT_NO_THROW(instance.Initialize());
    std::chrono::milliseconds dura( 1000 );
    std::this_thread::sleep_for(dura);
    EXPECT_NO_THROW(
        for(int i = 0 ; i < 10 ; ++i)
            instance.HandleMessage(msg.data());
    );
    ASSERT_EQ(respMsg.call, "zoomBy");
    EXPECT_TRUE(respMsg.data.empty());
    std::this_thread::sleep_for(dura);
}

TEST_F(NXEInstanceTest, zoomOut)
{
    std::string msg{ TestUtils::zoomByMessage(-2) };
    instance.registerMessageCallback(std::bind(&NXEInstanceTest::callback, this, std::placeholders::_1));
    EXPECT_NO_THROW(instance.Initialize());
    std::chrono::milliseconds dura( 1000 );
    std::this_thread::sleep_for(dura);
    EXPECT_NO_THROW(
        for(int i = 0 ; i < 10 ; ++i)
            instance.HandleMessage(msg.data());
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

TEST_F(NXEInstanceTest, zoomInAndOut)
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

    EXPECT_FALSE(respMsg.data.empty());
}

TEST_F(NXEInstanceTest, renderTest)
{
    std::string msg{ TestUtils::renderMessage() };
    render = true;
    instance.registerMessageCallback(std::bind(&NXEInstanceTest::callback, this, std::placeholders::_1));
    EXPECT_NO_THROW(instance.Initialize());
    std::chrono::milliseconds dura( 100 );
    std::this_thread::sleep_for(dura);
    EXPECT_NO_THROW(
        instance.HandleMessage(msg.data());
    );
    // Message cannot be properly parsed!
    EXPECT_TRUE(receivedRender);
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
    EXPECT_TRUE(respMsg.data.empty());
}

TEST_F(NXEInstanceTest, zoomedRenderBenchmarkTest)
{
    std::string msg{ TestUtils::renderMessage() };
    std::string zoomMsg{ TestUtils::zoomByMessage(8) };
    instance.registerMessageCallback(std::bind(&NXEInstanceTest::callback, this, std::placeholders::_1));
    EXPECT_NO_THROW(instance.Initialize());
    std::chrono::milliseconds dura( 100 );
    std::this_thread::sleep_for(dura);
    instance.HandleMessage(zoomMsg.data());
    EXPECT_NO_THROW(
        for(int i = 0 ; i < 20; ++i) {
            instance.HandleMessage(msg.data());
        }
    );
    EXPECT_TRUE(respMsg.data.empty());
}
