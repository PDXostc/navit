#include "navitdbus.h"
#include "log.h"
#include "navitprocessimpl.h"

#include <gtest/gtest.h>
#include <thread>
#include <chrono>

const std::string navitPath{ NAVIT_PATH };
extern bool runNavit;

struct NavitDBusTest : public ::testing::Test {
    NXE::NavitDBus connection;
    NXE::NavitProcessImpl process;

    void SetUp()
    {

        if (runNavit) {
            nDebug() << "Running navit binary";
            process.setProgramPath(navitPath);
            process.start();
        }
        connection.start();
    }

    void TearDown()
    {
        connection.stop();
        if (runNavit) {
            process.stop();
        }
    }
};

TEST_F(NavitDBusTest, zoom)
{
    EXPECT_NO_THROW(
        // wait a bit
        int defZoom = connection.zoom();
        nDebug() << "Zoom is " << defZoom;
        int zoomBy = 2;
        connection.zoomBy(2);

        EXPECT_EQ(defZoom / zoomBy, connection.zoom()););
}

TEST_F(NavitDBusTest, setOrientation)
{
    int orientation = connection.orientation();
    EXPECT_ANY_THROW(connection.setOrientation(1));

    if (orientation == 0) {
        connection.setOrientation(-1);
    }
    else {
        connection.setOrientation(0);
    }

    int newOrientation = connection.orientation();
    EXPECT_NE(orientation, newOrientation);
}

TEST_F(NavitDBusTest, setCenter)
{
    connection.setCenter(24.0,53.0);
}
