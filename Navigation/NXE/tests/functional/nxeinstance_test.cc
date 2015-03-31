#include "nxe_instance.h"
#include "navitprocessimpl.h"
#include "navitcontroller.h"
#include "navitdbus.h"
#include "testutils.h"

#include <gtest/gtest.h>
#include <memory>
#include <chrono>
#include <thread>
#include <fruit/fruit.h>

using namespace NXE;
extern bool runNavit;

typedef fruit::Component<INavitIPC, INavitProcess> NXEImpls;
struct NXEInstanceTest : public ::testing::Test {

    NXEInstance::DepInInterfaces injector { []() -> NXEImpls {
        return fruit::createComponent()
                .bind<INavitIPC, NavitDBus>()
                .bind<INavitProcess, NavitProcessImpl>();
        }() };
    NXEInstance instance {injector};
    JSONMessage respMsg;
    bool receivedRender {false};
    std::size_t numberOfResponses = 0;

    static void SetUpTestCase()
    {
        TestUtils::createNXEConfFile();
    }

    void callback(const std::string &response) {
        if (response.size() == 7171272) {
            receivedRender = true;
        }

        else {
            respMsg = NXE::JSONUtils::deserialize(response);
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
    zoom(2);
    ASSERT_EQ(respMsg.call, "zoomBy");
    EXPECT_TRUE(respMsg.data.empty());
}

// TODO: How to enable speech test?
TEST_F(NXEInstanceTest, DISABLED_speechTest) {
    // by default each time we want to draw a
    // speech 'draw' will be triggered
    instance.Initialize();
    zoom(2);
}

TEST_F(NXEInstanceTest, zoomOut)
{
    instance.registerMessageCallback(std::bind(&NXEInstanceTest::callback, this, std::placeholders::_1));
    EXPECT_NO_THROW(instance.Initialize());
    zoom(-2);
    ASSERT_EQ(respMsg.call, "zoomBy");
    EXPECT_TRUE(respMsg.data.empty());
}

TEST_F(NXEInstanceTest, zoom)
{
    std::string msg{ TestUtils::zoomMessage() };
    instance.registerMessageCallback(std::bind(&NXEInstanceTest::callback, this, std::placeholders::_1));
    EXPECT_NO_THROW(instance.Initialize());
    instance.HandleMessage(msg.data());

    EXPECT_EQ(respMsg.error.size(), 0);
    EXPECT_FALSE(respMsg.data.empty());
}

TEST_F(NXEInstanceTest, DISABLED_zoomInAndOut)
{
    std::string msg1{ TestUtils::zoomByMessage(2) };
    std::string msg2{ TestUtils::zoomByMessage(-2) };

    instance.registerMessageCallback(std::bind(&NXEInstanceTest::callback, this, std::placeholders::_1));
    EXPECT_NO_THROW(instance.Initialize());
    instance.HandleMessage(msg1.data());
    instance.HandleMessage(msg2.data());

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
    instance.HandleMessage(msg.data());
    std::vector<double> mes = instance.renderMeasurements();
    double mean = std::accumulate(mes.begin(), mes.end(), 0.0)/mes.size();
    perfLog("render") << " mean = " << mean;
    EXPECT_LT(mean, 400.0);
    // Message cannot be properly parsed!
    EXPECT_EQ(numberOfResponses,1);
}

TEST_F(NXEInstanceTest, moveByMessage)
{
    const std::string msg{ TestUtils::moveByMessage(10,10) };
    instance.registerMessageCallback(std::bind(&NXEInstanceTest::callback, this, std::placeholders::_1));
    EXPECT_NO_THROW(instance.Initialize());
    std::chrono::milliseconds dura( 100 );
    std::this_thread::sleep_for(dura);
    instance.HandleMessage(msg.data());
}

TEST_F(NXEInstanceTest, changeOrientation)
{
    const std::string msg{ TestUtils::changeOrientationMessage(-1) };
    const std::string msg2{ TestUtils::orientationMessage() };
    instance.registerMessageCallback(std::bind(&NXEInstanceTest::callback, this, std::placeholders::_1));
    EXPECT_NO_THROW(instance.Initialize());
    std::chrono::milliseconds dura( 100 );
    std::this_thread::sleep_for(dura);
    instance.HandleMessage(msg.data());
    instance.HandleMessage(msg2.data());

    EXPECT_EQ(numberOfResponses, 2);
    EXPECT_TRUE(respMsg.error.empty());
}

TEST_F(NXEInstanceTest, changeOrientationToIncorrectValue)
{
    const std::string msg{ TestUtils::changeOrientationMessage(100) };
    instance.registerMessageCallback(std::bind(&NXEInstanceTest::callback, this, std::placeholders::_1));
    EXPECT_NO_THROW(instance.Initialize());
    std::chrono::milliseconds dura( 100 );
    std::this_thread::sleep_for(dura);
    instance.HandleMessage(msg.data());

    EXPECT_FALSE(respMsg.error.empty());
}
