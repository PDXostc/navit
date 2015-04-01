#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include "gpsdprovider.h"

struct GPSDTest : public ::testing::Test {
};

// TODO : Can we make any positive test for this?
TEST_F(GPSDTest, startStop)
{
    NXE::GPSDProvider gpsd;
    std::chrono::milliseconds dura(1000);
    std::this_thread::sleep_for(dura);

    // altitude should be nan
    EXPECT_NE(gpsd.position().altitude, gpsd.position().altitude);
    // longitude should be nan
    EXPECT_NE(gpsd.position().longitude, gpsd.position().longitude);
    // latitude should be nan
    EXPECT_NE(gpsd.position().latitude, gpsd.position().latitude);
}
