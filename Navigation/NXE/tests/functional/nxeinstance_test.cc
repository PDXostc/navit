#include "nxe_instance.h"
#include "navitprocessimpl.h"
#include "navitcontroller.h"
#include "navitdbus.h"
#include "mapdownloaderdbus.h"
#include "gpsdprovider.h"
#include "testutils.h"
#include "dbuscontroller.h"

#include <gtest/gtest.h>
#include <memory>
#include <chrono>
#include <thread>
#include <fruit/fruit.h>

using namespace NXE;
extern bool runNavit;

struct NXEInstanceTest : public ::testing::Test {

    DBusController dbusController;
    INavitIPC * ipc { new NavitDBus{dbusController}};
    IMapDownloader * md { new MapDownloaderDBus{dbusController}};
    DI::Injector injector{ [this]() -> DI::Components {
        return fruit::createComponent()
                .bindInstance(*ipc)
                .bindInstance(*md)
                .bind<INavitProcess, NavitProcessImpl>()
                .bind<IGPSProvider, GPSDProvider>();
    }() };
    NXEInstance instance{ injector };
    JSONMessage respMsg;
    bool receivedRender{ false };
    std::size_t numberOfResponses = 0;

    static void SetUpTestCase()
    {
        TestUtils::createNXEConfFile();
    }

    void SetUp() override {
        instance.registerMessageCallback(std::bind(&NXEInstanceTest::callback, this, std::placeholders::_1));
        instance.Initialize();
        std::chrono::milliseconds dura(1000);
        std::this_thread::sleep_for(dura);
    }

    void callback(const std::string& response)
    {
        if (response.size() == 7171272) {
            receivedRender = true;
        }

        else {
            respMsg = NXE::JSONUtils::deserialize(response);
        }

        numberOfResponses++;
    }

    void zoom(int factor)
    {
        std::string msg{ TestUtils::zoomByMessage(factor) };
        instance.HandleMessage(msg.data());
    }
};

TEST_F(NXEInstanceTest, zoomByMessage_zoomIn_correct)
{
    zoom(2);
    ASSERT_EQ(respMsg.call, "zoomBy");
    EXPECT_TRUE(respMsg.data.empty());
    EXPECT_EQ(numberOfResponses, 1);
}

// TODO: How to enable speech test?
TEST_F(NXEInstanceTest, DISABLED_speechTest)
{
    // by default each time we want to draw a
    // speech 'draw' will be triggered
    instance.Initialize();
    zoom(2);
}

TEST_F(NXEInstanceTest, zoomByMessage_zoomOut_correct)
{
    zoom(-2);
    ASSERT_EQ(respMsg.call, "zoomBy");
    EXPECT_TRUE(respMsg.data.empty());
}

TEST_F(NXEInstanceTest, zoomMessage_correct)
{
    std::string msg{ TestUtils::zoomMessage() };
    instance.HandleMessage(msg.data());

    EXPECT_EQ(respMsg.error.size(), 0);
    EXPECT_FALSE(respMsg.data.empty());
}

TEST_F(NXEInstanceTest, zoomInAndOut)
{
    std::string msg1{ TestUtils::zoomByMessage(2) };
    std::string msg2{ TestUtils::zoomByMessage(-2) };

    instance.HandleMessage(msg1.data());
    instance.HandleMessage(msg2.data());

    EXPECT_TRUE(respMsg.data.empty());
    EXPECT_EQ(respMsg.call, "zoomBy");
}

TEST_F(NXEInstanceTest, renderMessage_correct)
{
    std::string msg{ TestUtils::renderMessage() };
    instance.HandleMessage(msg.data());
    std::vector<double> mes = instance.renderMeasurements();
    double mean = std::accumulate(mes.begin(), mes.end(), 0.0) / mes.size();
    perfLog("render") << " mean = " << mean;
    EXPECT_LT(mean, 400.0);
    // Message cannot be properly parsed!
    EXPECT_EQ(numberOfResponses, 1);
}

TEST_F(NXEInstanceTest, moveByMessage_correct)
{
    const std::string msg{ TestUtils::moveByMessage(10, 10) };
    std::chrono::milliseconds dura(100);
    std::this_thread::sleep_for(dura);
    instance.HandleMessage(msg.data());
}

TEST_F(NXEInstanceTest, changeOrientation_correct)
{
    const std::string msg{ TestUtils::changeOrientationMessage(-1) };
    const std::string msg2{ TestUtils::orientationMessage() };
    instance.HandleMessage(msg.data());
    instance.HandleMessage(msg2.data());

    EXPECT_EQ(numberOfResponses, 2);
    EXPECT_TRUE(respMsg.error.empty());
}

TEST_F(NXEInstanceTest, positionMessage_correct)
{
    const std::string msg{ TestUtils::positionMessage() };
    instance.HandleMessage(msg.data());
    EXPECT_TRUE(respMsg.error.empty());
}

TEST_F(NXEInstanceTest, changeOrientationMessage_incorrectValue)
{
    const std::string msg{ TestUtils::changeOrientationMessage(100) };
    instance.HandleMessage(msg.data());

    EXPECT_FALSE(respMsg.error.empty());
}

TEST_F(NXEInstanceTest, DISABLED_availableMessages_correct)
{
    const std::string msg{ TestUtils::availableMessages() };
    instance.registerMessageCallback(std::bind(&NXEInstanceTest::callback, this, std::placeholders::_1));
    EXPECT_NO_THROW(instance.Initialize());
    std::chrono::milliseconds dura(100);
    std::this_thread::sleep_for(dura);
    instance.HandleMessage(msg.data());

    EXPECT_TRUE(respMsg.error.empty());
    EXPECT_EQ(respMsg.call, "availableMaps");
    EXPECT_FALSE(respMsg.data.empty());
}

TEST_F(NXEInstanceTest, DISABLED_downloadMessage_incorrect_country)
{
    // Arrange
    const std::string msg{ TestUtils::downloadMessage("this country does not exists") };
    instance.registerMessageCallback(std::bind(&NXEInstanceTest::callback, this, std::placeholders::_1));
    EXPECT_NO_THROW(instance.Initialize());
    std::chrono::milliseconds dura(100);
    std::this_thread::sleep_for(dura);

    // Act
    instance.HandleMessage(msg.data());

    // Assert
    EXPECT_FALSE(respMsg.error.empty());
}

TEST_F(NXEInstanceTest, DISABLED_downloadMessage_correct_country)
{
    // Arrange
    const std::string msg{ TestUtils::downloadMessage("Hawaii") };
    const std::string msg2{ TestUtils::cancelDownloadMessage("Hawaii") };
    instance.registerMessageCallback(std::bind(&NXEInstanceTest::callback, this, std::placeholders::_1));
    EXPECT_NO_THROW(instance.Initialize());
    std::chrono::milliseconds dura(100);
    std::this_thread::sleep_for(dura);

    // Act
    instance.HandleMessage(msg.data());

    while (true) {
        std::this_thread::sleep_for(dura);
    }

    // Assert
    EXPECT_TRUE(respMsg.error.empty());
    EXPECT_EQ(respMsg.call, "downloadMap");

    instance.HandleMessage(msg2.data());

}
