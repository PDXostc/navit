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
#include <fruit/fruit.h>

using namespace NXE;
extern bool runNavit;

typedef fruit::Component<INavitIPC, INavitProcess, IGPSProvider> NXEImpls;
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
                .bind<ISpeech,SpeechMock>();
    }() };
    NXEInstance instance{ injector };
    bool receivedRender{ false };
    std::size_t numberOfResponses = 0;

    static void SetUpTestCase()
    {
        TestUtils::createNXEConfFile();
    }

    void SetUp() override {
        instance.Initialize();
        std::chrono::milliseconds dura(1000);
        std::this_thread::sleep_for(dura);
    }

    void setDestination(double lon, double lat, const char* desc)
    {
         instance.HandleMessage<SetDestinationMessageTag>(lon, lat, desc);
    }

    void setPosition(double lon, double lat)
    {
         instance.HandleMessage<SetPositionMessageTag>(lon, lat);
    }

    void clearDestination()
    {
        instance.HandleMessage<ClearDestinationMessageTag>();
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
}

