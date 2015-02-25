
#include <gtest/gtest.h>
#include "log.h"


struct TestListener : public ::testing::EmptyTestEventListener
{
    void OnTestStart(const ::testing::TestInfo &info) override {
        nDebug() << "Test " << info.name() << " started";
    }

    void OnTestEnd(const ::testing::TestInfo &info) override {
        nDebug() << "Test " << info.name() << " finished";
    }
};

int main(int argc, char* argv[])
{
    auto logger = spdlog::stdout_logger_mt("nxe_logger");
    logger->set_level(spdlog::level::debug);
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::UnitTest::GetInstance()->listeners().Append(new TestListener);
    return RUN_ALL_TESTS();
}
