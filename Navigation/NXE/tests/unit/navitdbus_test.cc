#include <gtest/gtest.h>
#include "navitdbus.h"
#include "log.h"
#include <thread>
#include <chrono>

struct NavitDBusTest : public ::testing::Test
{
};

TEST_F(NavitDBusTest, test)
{
    EXPECT_NO_THROW(
    NXE::NavitDBus connection;
    connection.start();

    // wait a bit
    std::chrono::milliseconds dura( 2000 );

    std::this_thread::sleep_for(dura);
    connection.stop();
    );
}
