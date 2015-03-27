#include "navitprocessimpl.h"
#include "nxe_extension.h"
#include <navitdbus.h>

#include <iostream>
#include <chrono>
#include <thread>

#include <gtest/gtest.h>

const std::string navitPath { NAVIT_PATH };
extern bool runNavit;

class NavitProcessTest : public ::testing::Test
{
protected:
    NXE::NavitProcessImpl process;
    NXE::NavitDBus dbus;
};

TEST_F(NavitProcessTest, failure_start_pathNotSet)
{
    if (runNavit) {
        EXPECT_FALSE(process.start());
        EXPECT_FALSE(process.isRunning());
    }
}

TEST_F(NavitProcessTest, success_start_pathSet)
{
    if (runNavit) {
        process.setProgramPath(navitPath);
        EXPECT_TRUE(process.start());
        EXPECT_TRUE(process.isRunning());
    }
    dbus.start();
    dbus.stop(true);
}
