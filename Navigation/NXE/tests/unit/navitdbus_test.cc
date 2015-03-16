#include "navitdbus.h"
#include "log.h"
#include "navitprocessimpl.h"

#include <gtest/gtest.h>
#include <thread>
#include <chrono>

NXE::NavitProcessImpl *process = nullptr;
const std::string navitPath { NAVIT_PATH };

struct NavitDBusTest : public ::testing::Test
{
    NXE::NavitDBus connection;

    static void SetUpTestCase() {
        nDebug() << "Running navit binary";
        process = new NXE::NavitProcessImpl;
        process->setProgramPath(navitPath);
        process->start();
        std::chrono::milliseconds dura( 1000 );
        std::this_thread::sleep_for(dura);
    }

    static void TearDownTestCase() {
        process->stop();
        delete process;
        process = 0;
    }
};

TEST_F(NavitDBusTest, properStart)
{
    ASSERT_NO_THROW(connection.start());
    EXPECT_NO_THROW(

        // wait a bit
        std::chrono::milliseconds dura( 1000 );
        std::this_thread::sleep_for(dura);
        connection.stop();
    );
}

TEST_F(NavitDBusTest, zoom)
{
    ASSERT_NO_THROW(connection.start());
    EXPECT_NO_THROW(
        // wait a bit
        std::chrono::milliseconds dura( 1000 );

        int defZoom = connection.zoom();
        nDebug() << "Zoom is " << defZoom;
        int zoomBy = 2;
        connection.zoomBy(2);

        EXPECT_EQ(defZoom / zoomBy, connection.zoom());
        std::this_thread::sleep_for(dura);

        connection.stop();
    );
}
