#include "benchmark/benchmark.h"

#include "nxe_instance.h"
#include "context.h"
#include "navitprocessimpl.h"
#include "navitcontroller.h"
#include "inavitipc.h"
#include "navitdbus.h"
#include "testutils.h"
#include "mocks/gpsmock.h"
#include "mocks/mapdownloadermock.h"
#include "dbuscontroller.h"
#include "speechimpl.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <chrono>
#include <thread>
#include <fruit/component.h>
#include <fruit/injector.h>
#include <gmock/gmock.h>

namespace bpt = boost::property_tree;

struct RenderTest {
    NXE::DBusController dbusController;
    NXE::INavitIPC * ipc { new NXE::NavitDBus{dbusController}};
    NXE::DI::Injector injector{ [this]() -> NXE::DI::Components {
        return fruit::createComponent()
                .bindInstance(*ipc)
                .bind<NXE::INavitProcess, NXE::NavitProcessImpl>()
                .bind<NXE::IGPSProvider, GPSMock>()
                .bind<NXE::IMapDownloader, MapDownloaderMock>()
                .bind<NXE::ISpeech,NXE::SpeechImpl>();
    }() };
    NXE::NXEInstance instance{ injector };
    MapDownloaderMock* mock_mapd{ (dynamic_cast<MapDownloaderMock*>(injector.get<NXE::IMapDownloader*>())) };
};

void renderOneFrame(benchmark::State& state)
{
    RenderTest t;
    EXPECT_CALL(*(t.mock_mapd), setListener(::testing::_));

    t.instance.Initialize();
    std::chrono::milliseconds dura(100);
    std::this_thread::sleep_for(dura);
    while (state.KeepRunning()) {
        t.instance.HandleMessage(TestUtils::renderMessage());
    }
}

void moveBackAndForth(benchmark::State& state)
{
    bpt::ptree backTree, fwdTree;
    backTree.put("x", 500);
    backTree.put("y", 900);
    fwdTree.put("x", 500);
    fwdTree.put("y", 900);

    RenderTest t;
    EXPECT_CALL(*(t.mock_mapd), setListener(::testing::_));
    t.instance.Initialize();
    std::chrono::milliseconds dura(100);
    std::this_thread::sleep_for(dura);

    while (state.KeepRunning()) {
        t.instance.HandleMessage(NXE::JSONMessage{ 3, "moveBy", "", backTree });
        t.instance.HandleMessage(NXE::JSONMessage{ 3, "moveBy", "", fwdTree });
    }
}

BENCHMARK(renderOneFrame);
BENCHMARK(moveBackAndForth);
