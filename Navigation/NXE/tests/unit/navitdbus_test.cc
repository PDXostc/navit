#include "navitdbus.h"
#include "log.h"
#include "navitprocessimpl.h"
#include "testutils.h"
#include "dbuscontroller.h"

#include <gtest/gtest.h>
#include <thread>
#include <chrono>

const std::string navitPath{ NAVIT_PATH };
extern bool runNavit;

struct NavitDBusTest : public ::testing::Test {

    NXE::DBusController controller;
    NXE::NavitDBus connection {controller};
    NXE::NavitProcessImpl process;

    void SetUp()
    {
        TestUtils::createNXEConfFile();
        if (runNavit) {
            nDebug() << "Running navit binary";
            // TODO: Add settings here
            process.start();
            std::chrono::milliseconds dura(500);
            std::this_thread::sleep_for(dura);
        }
        nTrace() << "Controller start";
        controller.start();
    }

    void TearDown()
    {
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

TEST_F(NavitDBusTest, setPosition)
{
    connection.setPosition(24.0,53.0);
}

TEST_F(NavitDBusTest, setDestination)
{
    connection.setDestination(24.10,53.10,"dest1");
}

TEST_F(NavitDBusTest, clearDestination)
{
    connection.clearDestination();
}

TEST_F(NavitDBusTest, setScheme)
{
    EXPECT_NO_THROW(
    connection.setScheme("Car-JLR"));

    EXPECT_NO_THROW(
    connection.setScheme("Car-JLR-nopoi"));
}

TEST_F(NavitDBusTest, DISABLED_setScheme_failure)
{
    EXPECT_ANY_THROW(
    connection.setScheme("Car-JLR-not-exists"));
}
