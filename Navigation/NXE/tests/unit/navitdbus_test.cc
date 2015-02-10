
#include <gtest/gtest.h>
#include "navitdbus.h"
#include <thread>
#include <chrono>

struct NavitDBusTest : public ::testing::Test
{
};

TEST_F(NavitDBusTest, test)
{
    NXE::NavitDBus connection;
    connection.start();

    // wait a bit
    std::chrono::milliseconds dura( 2000 );

    std::this_thread::sleep_for(dura);
    connection.stop();
}
