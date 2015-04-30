#include "benchmark/benchmark.h"

#include "nxe_instance.h"
#include "context.h"
#include "navitprocessimpl.h"
#include "inavitipc.h"
#include "navitdbus.h"
#include "testutils.h"
#include "mocks/gpsmock.h"
#include "mocks/mapdownloadermock.h"
#include "dbuscontroller.h"
#include "mocks/speechmock.h"

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
                .bind<NXE::ISpeech,SpeechMock>();
    }() };
    NXE::NXEInstance instance{ injector };
    MapDownloaderMock* mock_mapd{ (dynamic_cast<MapDownloaderMock*>(injector.get<NXE::IMapDownloader*>())) };
};

void renderOneFrame(benchmark::State& state)
{
    RenderTest t;

    t.instance.Initialize();
    std::chrono::milliseconds dura(100);
    std::this_thread::sleep_for(dura);
    while (state.KeepRunning()) {
        t.instance.HandleMessage<RenderMessageTag>();
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
    t.instance.Initialize();
    std::chrono::milliseconds dura(100);
    std::this_thread::sleep_for(dura);

    while (state.KeepRunning()) {
        t.instance.HandleMessage<MoveByMessageTag>(500,900);
        t.instance.HandleMessage<MoveByMessageTag>(500,900);
    }
}

BENCHMARK(renderOneFrame);
BENCHMARK(moveBackAndForth);
