#include "nxe_instance.h"
#include "navitprocessimpl.h"
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

using namespace NXE;
extern bool runNavit;

struct NXEInstanceTest : public ::testing::Test {

    DBusController dbusController;
    INavitIPC * ipc { new NavitDBus{dbusController}};
    IMapDownloader * md { new MapDownloaderDBus{dbusController}};
    NXE::DI::Injector injector{ std::make_tuple(
                    std::shared_ptr<NXE::INavitIPC>(ipc),
                    std::shared_ptr<NXE::INavitProcess>(new NXE::NavitProcessImpl),
                    std::shared_ptr<NXE::IGPSProvider>(new GPSDProvider),
                    std::shared_ptr<NXE::IMapDownloader>(md),
                    std::shared_ptr<NXE::ISpeech>(new SpeechMock)
                    )};
    NXEInstance instance{ injector };
    bool receivedRender{ false };
    std::size_t numberOfResponses = 0;

    static void SetUpTestCase()
    {
        TestUtils::createNXEConfFile();
    }

    void SetUp() override {
        dbusController.start();
        instance.Initialize();
        std::chrono::milliseconds dura(1000);
        std::this_thread::sleep_for(dura);
    }

    void setDestination(double lon, double lat, const char* desc)
    {
         instance.startNavigation(lon, lat, desc);
    }

    void setPosition(double lon, double lat)
    {
         instance.ipc()->setPosition(lon, lat);
    }

    void clearDestination()
    {
         instance.cancelNavigation();
    }

    void addWaypoint(double lon, double lat)
    {
        instance.ipc()->addWaypoint(lon, lat);
    }
};

TEST_F(NXEInstanceTest, Routing)
{

    setPosition(11.5659, 48.1392);

    std::chrono::milliseconds dura_1s(1000);
    std::this_thread::sleep_for(dura_1s);

    setDestination(11.5775, 48.1427, "1");

    std::chrono::milliseconds dura_5s(5000);
    std::this_thread::sleep_for(dura_5s);

    clearDestination();
    setDestination(11.5875, 48.1527, "2");

    std::this_thread::sleep_for(dura_5s);

    setPosition(11.5859, 48.1692);

    std::this_thread::sleep_for(dura_1s);

    setDestination(11.5975, 48.1727, "3");

    std::this_thread::sleep_for(dura_5s);

    clearDestination();

    setPosition(11.5659, 48.1392);

    setDestination(11.5775, 48.1427, "1");

    std::this_thread::sleep_for(dura_1s);

    addWaypoint(11.586, 48.149);

    std::this_thread::sleep_for(dura_5s);

    clearDestination();
}

TEST_F(NXEInstanceTest, Waypoints)
{
    std::chrono::milliseconds dura_5s(5000);

    clearDestination();

    setPosition(11.5659, 48.1392);

    setDestination(11.5775, 48.1427, "1");

    std::this_thread::sleep_for(dura_5s);

    addWaypoint(11.586, 48.149);

    std::this_thread::sleep_for(dura_5s);

    clearDestination();
}


TEST_F(NXEInstanceTest, Routing_Portland)
{
    std::chrono::milliseconds dura_5s(5000);
    std::this_thread::sleep_for(dura_5s);
    clearDestination();
    setDestination(-122.579,45.5621, "1");
    std::this_thread::sleep_for(dura_5s);
    EXPECT_TRUE(instance.ipc()->isNavigationRunning());
    std::this_thread::sleep_for(dura_5s);
    std::this_thread::sleep_for(dura_5s);

    clearDestination();
}


TEST_F(NXEInstanceTest, Routing_Wrong_Portland)
{
    std::chrono::milliseconds dura_5s(5000);
    std::this_thread::sleep_for(dura_5s);
    clearDestination();
    setDestination(53,45.5621, "1");
    std::this_thread::sleep_for(dura_5s);
    EXPECT_TRUE(instance.ipc()->isNavigationRunning());
    std::this_thread::sleep_for(dura_5s);
    std::this_thread::sleep_for(dura_5s);

    clearDestination();
}
