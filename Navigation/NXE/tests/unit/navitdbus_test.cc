#include <gtest/gtest.h>
#include "navitdbus.h"
#include "log.h"
#include <thread>
#include <chrono>

struct NavitDBusTest : public ::testing::Test
{
    NXE::NavitDBus connection;
};

TEST_F(NavitDBusTest, properStart)
{
    EXPECT_NO_THROW(
        connection.start();

        // wait a bit
        std::chrono::milliseconds dura( 2000 );

        std::this_thread::sleep_for(dura);
        connection.stop();
    );
}

TEST_F(NavitDBusTest, zoom)
{
    EXPECT_NO_THROW(
        connection.start();
        // wait a bit
        std::chrono::milliseconds dura( 1000 );
        std::this_thread::sleep_for(dura);

        int defZoom = connection.zoom();
        nDebug() << "Zoom is " << defZoom;
        int zoomBy = 2;
        connection.zoomBy(2);

        EXPECT_EQ(defZoom / zoomBy, connection.zoom());
        std::this_thread::sleep_for(dura);

        connection.stop();
    );
}
