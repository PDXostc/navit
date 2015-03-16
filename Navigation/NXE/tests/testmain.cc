#include <gtest/gtest.h>
#include "log.h"

struct TestListener : public ::testing::EmptyTestEventListener
{
    void OnTestStart(const ::testing::TestInfo &info) override {
    }

    void OnTestEnd(const ::testing::TestInfo &info) override {
    }
};

int main(int argc, char* argv[])
{
    const std::vector<std::string> arguments(argv + 1, argv + argc);

    bool debug = std::find(arguments.begin(), arguments.end(), "--debug") != arguments.end();

    auto logger = spdlog::stdout_logger_mt("nxe");
    auto perfLogger = spdlog::stdout_logger_mt("perf");
    perfLogger->set_level(spdlog::level::info);
    if (debug)
        logger->set_level(spdlog::level::trace);
    else
        logger->set_level(spdlog::level::err);
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::UnitTest::GetInstance()->listeners().Append(new TestListener);
    return RUN_ALL_TESTS();
}
