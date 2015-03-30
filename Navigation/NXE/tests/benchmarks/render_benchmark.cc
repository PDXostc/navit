#include "benchmark/benchmark.h"

#include "nxe_instance.h"
#include "navitprocessimpl.h"
#include "navitcontroller.h"
#include "inavitipc.h"
#include "navitdbus.h"
#include "testutils.h"
#include "mocks/gpsmock.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <chrono>
#include <thread>
#include <fruit/component.h>
#include <fruit/injector.h>


namespace bpt = boost::property_tree;

struct RenderTest {
    NXE::DI::Injector injector{ []() -> NXE::DI::Components {
        return fruit::createComponent()
                .bind<NXE::INavitIPC, NXE::NavitDBus>()
                .bind<NXE::INavitProcess, NXE::NavitProcessImpl>()
                .bind<NXE::IGPSProvider, GPSMock>();
    }() };
    NXE::NXEInstance instance{ injector };
};

void renderOneFrame(benchmark::State& state)
{
    const NXE::JSONMessage msg{ 3, "render" };
    const std::string sMsg{ NXE::JSONUtils::serialize(msg) };

    RenderTest t;
    t.instance.Initialize();
    std::chrono::milliseconds dura(100);
    std::this_thread::sleep_for(dura);
    while (state.KeepRunning()) {
        t.instance.HandleMessage(sMsg.data());
    }
}

void moveBackAndForth(benchmark::State& state)
{
    bpt::ptree backTree, fwdTree;
    backTree.put("x", 500);
    backTree.put("y", 900);
    fwdTree.put("x", 1080);
    fwdTree.put("y", 1900);
    const std::string back{ NXE::JSONUtils::serialize(NXE::JSONMessage{ 3, "moveBy", "", backTree }) };
    const std::string forth{ NXE::JSONUtils::serialize(NXE::JSONMessage{ 3, "moveBy", "", fwdTree }) };

    RenderTest t;
    t.instance.Initialize();
    std::chrono::milliseconds dura(100);
    std::this_thread::sleep_for(dura);

    while (state.KeepRunning()) {
        t.instance.HandleMessage(back.data());
        t.instance.HandleMessage(forth.data());
    }
}

BENCHMARK(renderOneFrame);
BENCHMARK(moveBackAndForth);
