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

struct NXEMultithreadingTest : public ::testing::Test {

    DBusController dbusController;
    INavitIPC * ipc { new NavitDBus{dbusController}};
    IMapDownloader * md { new MapDownloaderDBus{dbusController}};
    NXE::DI::Injector injector{ std::make_tuple(
                    std::shared_ptr<NXE::INavitIPC>(ipc),
                    std::shared_ptr<NXE::INavitProcess>(new NXE::NavitProcessImpl),
                    std::shared_ptr<NXE::IGPSProvider>(new GPSDProvider),
                    std::shared_ptr<NXE::IMapDownloader>(new MapDownloaderDBus{dbusController}),
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
        instance.Initialize();
        std::chrono::milliseconds dura(1000);
        std::this_thread::sleep_for(dura);
    }

    void zoom(int factor)
    {
        instance.ipc()->setZoom(factor);
    }
};


TEST_F(NXEMultithreadingTest, simple_multithreaded)
{
    auto renderThread = std::thread { [this]() {
        instance.ipc()->render();;

    }};
    auto setSchemeThread = std::thread { [this]() {
        instance.ipc()->setScheme("");
    }};
    auto setPositionThread = std::thread { [this]() {
        instance.ipc()->setPosition(0,0);
    }};
    auto orientationThread = std::thread { []() {}};
//    auto zoomByThread = std::thread { []() {}};
//    auto resizeThread = std::thread { []() {}};

    renderThread.join();
    setSchemeThread.join();
    setPositionThread.join();
    orientationThread.join();

}
