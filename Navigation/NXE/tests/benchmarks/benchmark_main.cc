
#include <vector>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "log.h"
#include "settingtags.h"
#include "benchmark/benchmark.h"

namespace bpt = boost::property_tree;
const std::string navitPath{ NAVIT_PATH };

bool runNavit = true;
int main(int argc, char** argv)
{
    const std::vector<std::string> arguments{ argv + 1, argv + argc };

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

    runNavit = !externalServer;
    if (externalServer) {
        config.put(SettingsTags::Navit::ExternalNavit::name(), true);
    }
    else {
        config.put(SettingsTags::Navit::ExternalNavit::name(), false);
    }

    config.put(SettingsTags::FileLog::name(), "/tmp/log.file");
    bpt::write_json("nxe.conf", config);

    benchmark::Initialize(&argc, const_cast<const char**>(argv));
    benchmark::RunSpecifiedBenchmarks();
    return 0;
}
