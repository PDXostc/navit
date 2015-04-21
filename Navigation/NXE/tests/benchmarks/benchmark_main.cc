
#include <vector>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "log.h"
#include "settingtags.h"
#include "benchmark/benchmark.h"
#include "testutils.h"

bool runNavit = true;
int main(int argc, char** argv)
{
    const std::vector<std::string> arguments{ argv + 1, argv + argc };

    bool debug = std::find(arguments.begin(), arguments.end(), "--debug") != arguments.end();
    bool externalServer = std::find(arguments.begin(), arguments.end(), "--no-navit") != arguments.end();

    auto logger = spdlog::stdout_logger_mt("nxe");
    auto perfLogger = spdlog::stdout_logger_mt("perf");
    if (debug) {
        logger->set_level(spdlog::level::trace);
        perfLogger->set_level(spdlog::level::info);
    }
    else {
        logger->set_level(spdlog::level::off);
        perfLogger->set_level(spdlog::level::off);
    }

    runNavit = !externalServer;

    TestUtils::createNXEConfFile();

    benchmark::Initialize(&argc, const_cast<const char**>(argv));
    benchmark::RunSpecifiedBenchmarks();

    TestUtils::removeNXEConfFile();

    return 0;
}
