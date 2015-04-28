#include "nxe_instance.h"
#include "navitprocessimpl.h"
#include "navitcontroller.h"
#include "navitdbus.h"
#include "mapdownloaderdbus.h"
#include "gpsdprovider.h"
#include "testutils.h"
#include "dbuscontroller.h"
#include "mocks/speechmock.h"

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
                .bind<IGPSProvider, GPSDProvider>()
                // We have to use speech mock here, as there's no
                // SRS service on Linux
                .bind<ISpeech,SpeechMock>();
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

    void callback(const JSONMessage& rsp)
    {
        respMsg = rsp;
        numberOfResponses++;
    }

    void zoom(int factor)
    {
        instance.HandleMessage(TestUtils::zoomByMessage(factor));
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
    instance.HandleMessage(TestUtils::zoomMessage());

    EXPECT_EQ(respMsg.error.size(), 0);
    EXPECT_FALSE(respMsg.data.empty());
}

TEST_F(NXEInstanceTest, zoomInAndOut)
{

    instance.HandleMessage(TestUtils::zoomByMessage(2));
    instance.HandleMessage(TestUtils::zoomByMessage(-2));

    EXPECT_TRUE(respMsg.data.empty());
    EXPECT_EQ(respMsg.call, "zoomBy");
}

TEST_F(NXEInstanceTest, renderMessage_correct)
{
    instance.HandleMessage(TestUtils::renderMessage());
    // Message cannot be properly parsed!
    EXPECT_EQ(numberOfResponses, 1);
}

TEST_F(NXEInstanceTest, moveByMessage_correct)
{
    std::chrono::milliseconds dura(100);
    std::this_thread::sleep_for(dura);
    instance.HandleMessage(TestUtils::moveByMessage(10, 10));
}

TEST_F(NXEInstanceTest, changeOrientation_correct)
{
    instance.HandleMessage(TestUtils::changeOrientationMessage(-1));
    instance.HandleMessage(TestUtils::orientationMessage());

    EXPECT_EQ(numberOfResponses, 2);
    EXPECT_TRUE(respMsg.error.empty());
}

TEST_F(NXEInstanceTest, positionMessage_correct)
{
    instance.HandleMessage(TestUtils::positionMessage());
    EXPECT_TRUE(respMsg.error.empty());
}

TEST_F(NXEInstanceTest, changeOrientationMessage_incorrectValue)
{
    const std::string msg{  };
    instance.HandleMessage(TestUtils::changeOrientationMessage(100));

    EXPECT_FALSE(respMsg.error.empty());
}

TEST_F(NXEInstanceTest, DISABLED_availableMessages_correct)
{
    instance.registerMessageCallback(std::bind(&NXEInstanceTest::callback, this, std::placeholders::_1));
    EXPECT_NO_THROW(instance.Initialize());
    std::chrono::milliseconds dura(100);
    std::this_thread::sleep_for(dura);
    instance.HandleMessage(TestUtils::availableMessages());

    EXPECT_TRUE(respMsg.error.empty());
    EXPECT_EQ(respMsg.call, "availableMaps");
    EXPECT_FALSE(respMsg.data.empty());
}

TEST_F(NXEInstanceTest, DISABLED_downloadMessage_incorrect_country)
{
    // Arrange
    instance.registerMessageCallback(std::bind(&NXEInstanceTest::callback, this, std::placeholders::_1));
    EXPECT_NO_THROW(instance.Initialize());
    std::chrono::milliseconds dura(100);
    std::this_thread::sleep_for(dura);

    // Act
    instance.HandleMessage(TestUtils::downloadMessage("this country does not exists"));

    // Assert
    EXPECT_FALSE(respMsg.error.empty());
}

TEST_F(NXEInstanceTest, DISABLED_downloadMessage_correct_country)
{
    // Arrange
    instance.registerMessageCallback(std::bind(&NXEInstanceTest::callback, this, std::placeholders::_1));
    EXPECT_NO_THROW(instance.Initialize());
    std::chrono::milliseconds dura(100);
    std::this_thread::sleep_for(dura);

    // Act
    instance.HandleMessage(TestUtils::downloadMessage("Hawaii"));

    while (true) {
        std::this_thread::sleep_for(dura);
    }

    // Assert
    EXPECT_TRUE(respMsg.error.empty());
    EXPECT_EQ(respMsg.call, "downloadMap");

    instance.HandleMessage(TestUtils::cancelDownloadMessage("Hawaii"));

}
