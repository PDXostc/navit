
#include <gtest/gtest.h>
#include "navitdbus.h"

struct NavitDBusTest : public ::testing::Test
{};

TEST_F(NavitDBusTest, test)
{
    NXE::NavitDBus connection;
    connection.start();

    // wait a bit

    connection.stop();
}
