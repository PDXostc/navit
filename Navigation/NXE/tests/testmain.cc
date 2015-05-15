#include <gtest/gtest.h>
#include "log.h"

struct TestListener : public ::testing::EmptyTestEventListener {
    void OnTestStart(const ::testing::TestInfo& info) override
    {
    }

    void OnTestEnd(const ::testing::TestInfo& info) override
    {
    }
};

bool runNavit = true;

int main(int argc, char* argv[])
{
    const std::vector<std::string> arguments{ argv + 1, argv + argc };

    bool debug = std::find(arguments.begin(), arguments.end(), "--debug") != arguments.end();
    bool perf = std::find(arguments.begin(), arguments.end(), "--perf") != arguments.end();
    bool extNavit = std::find(arguments.begin(), arguments.end(), "--no-navit") != arguments.end();

    auto logger = spdlog::stdout_logger_mt("nxe");
    auto dbusLogger = spdlog::stdout_logger_mt("dbus");
    dbusLogger->set_level(spdlog::level::trace);
    auto perfLogger = spdlog::stdout_logger_mt("perf");
    if (perf) {
        perfLogger->set_level(spdlog::level::info);
    } else {
        perfLogger->set_level(spdlog::level::off);
    }
    if (debug) {
        logger->set_level(spdlog::level::trace);
        dbusLogger->set_level(spdlog::level::trace);
        perfLogger->set_level(spdlog::level::info);
    }
    else {
        logger->set_level(spdlog::level::off);
    }

    if (extNavit) {
        runNavit = false;
    }
    spdlog::set_pattern("[%H:%M:%S.%e] [%n] [%t] [%l] %v");

    ::testing::InitGoogleTest(&argc, argv);
    ::testing::UnitTest::GetInstance()->listeners().Append(new TestListener);
    return RUN_ALL_TESTS();
}
