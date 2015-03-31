#include "benchmark/benchmark.h"

#include "nxe_instance.h"
#include "navitprocessimpl.h"
#include "navitcontroller.h"
#include "inavitipc.h"
#include "navitdbus.h"
#include "testutils.h"
#include "settingtags.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <chrono>
#include <thread>
#include <fruit/component.h>
#include <fruit/injector.h>

namespace bpt = boost::property_tree;

const std::string navitPath { NAVIT_PATH };

struct RenderTest {
    NXE::NXEInstance::DepInInterfaces injector { []() -> fruit::Component<NXE::INavitIPC, NXE::INavitProcess> {
        return fruit::createComponent()
                .bind<NXE::INavitIPC, NXE::NavitDBus>()
                .bind<NXE::INavitProcess, NXE::NavitProcessImpl>();
        }() };
    NXE::NXEInstance instance{injector};
};

void renderOneFrame(benchmark::State &state)
{
    const NXE::JSONMessage msg { 3, "render"};
    const std::string sMsg {NXE::JSONUtils::serialize(msg)};

    RenderTest t;
    t.instance.Initialize();
    std::chrono::milliseconds dura( 100 );
    std::this_thread::sleep_for(dura);
    while(state.KeepRunning()) {
        t.instance.HandleMessage(sMsg.data());
    }
}

void moveBackAndForth(benchmark::State &state)
{
    bpt::ptree backTree, fwdTree;
    backTree.put("x", 500); backTree.put("y",900);
    fwdTree.put("x", 1080); fwdTree.put("y",1900);
    const std::string back { NXE::JSONUtils::serialize( NXE::JSONMessage {3,"moveBy", "", backTree }) };
    const std::string forth { NXE::JSONUtils::serialize( NXE::JSONMessage {3,"moveBy", "", fwdTree }) };

    RenderTest t;
    t.instance.Initialize();
    std::chrono::milliseconds dura( 100 );
    std::this_thread::sleep_for(dura);

    while (state.KeepRunning()) {
        t.instance.HandleMessage(back.data());
        t.instance.HandleMessage(forth.data());
    }
}

BENCHMARK(renderOneFrame);
BENCHMARK(moveBackAndForth);

int main(int argc, char **argv)
{
    const std::vector<std::string> arguments {argv + 1, argv + argc};

    bpt::ptree config;
    config.put(SettingsTags::Navit::Path::name(), navitPath);
    config.put(SettingsTags::Navit::AutoStart::name(), true);

    bool debug = std::find(arguments.begin(), arguments.end(), "--debug") != arguments.end();
    bool perf = std::find(arguments.begin(), arguments.end(), "--perf") != arguments.end();
    bool externalServer = std::find(arguments.begin(), arguments.end(), "--no-navit") != arguments.end();

    auto logger = spdlog::stdout_logger_mt("nxe");
    auto perfLogger = spdlog::stdout_logger_mt("perf");
    if (perf)
        perfLogger->set_level(spdlog::level::info);
    else
        perfLogger->set_level(spdlog::level::off);
    if (debug) {
        logger->set_level(spdlog::level::trace);
        perfLogger->set_level(spdlog::level::info);
    }
    else {
        logger->set_level(spdlog::level::off);
    }

    if (externalServer) {
        config.put(SettingsTags::Navit::ExternalNavit::name(), true);
    } else {
        config.put(SettingsTags::Navit::ExternalNavit::name(), false);
    }

    config.put(SettingsTags::FileLog::name(), "/tmp/log.file");
    bpt::write_json("nxe.conf", config);

    benchmark::Initialize(&argc, const_cast<const char**>(argv));
    benchmark::RunSpecifiedBenchmarks();
    return 0;
}
