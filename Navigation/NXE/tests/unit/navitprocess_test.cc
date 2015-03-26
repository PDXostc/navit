#include "navitprocessimpl.h"
#include "nxe_extension.h"

#include <iostream>
#include <chrono>
#include <thread>

#include <gtest/gtest.h>

const std::string navitPath { NAVIT_PATH };

class NavitProcessTest : public ::testing::Test
{
protected:
    NXE::NavitProcessImpl process;
};

TEST_F(NavitProcessTest, failure_start_pathNotSet)
{
    EXPECT_FALSE(process.start());
    EXPECT_FALSE(process.isRunning());
}

TEST_F(NavitProcessTest, success_start_pathSet)
{
    process.setProgramPath(navitPath);
    EXPECT_TRUE(process.start());
    EXPECT_TRUE(process.isRunning());
}
