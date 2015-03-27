#include "navitdbus.h"
#include "log.h"
#include "navitprocessimpl.h"

#include <gtest/gtest.h>
#include <thread>
#include <chrono>

NXE::NavitProcessImpl *process = nullptr;
const std::string navitPath { NAVIT_PATH };
extern bool runNavit;

struct NavitDBusTest : public ::testing::Test
{
    NXE::NavitDBus connection;

    static void SetUpTestCase() {
        if (runNavit) {
            nDebug() << "Running navit binary";
            process = new NXE::NavitProcessImpl;
            process->setProgramPath(navitPath);
            process->start();
        }
    }

    static void TearDownTestCase() {
        if (runNavit) {
            delete process;
            process = 0;
        }
    }

    void SetUp() {
        ASSERT_NO_THROW(connection.start());
    }

    void TearDown() {
        connection.stop(runNavit);
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

        EXPECT_EQ(defZoom / zoomBy, connection.zoom());
    );
}

TEST_F(NavitDBusTest, setOrientation)
{
    int orientation = connection.orientation();
    EXPECT_ANY_THROW(connection.setOrientation(1));

    if (orientation == 0) {
        connection.setOrientation(-1);
    } else {
        connection.setOrientation(0);
    }


    int newOrientation = connection.orientation();

    EXPECT_NE(orientation, newOrientation);
}
