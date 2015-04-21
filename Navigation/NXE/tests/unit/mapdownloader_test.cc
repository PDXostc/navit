#include <gtest/gtest.h>
#include <chrono>
#include <thread>

#include "mapdownloaderdbus.h"
#include "dbuscontroller.h"
#include "log.h"

struct MapDownloaderDBusTest : public ::testing::Test 
{
    void SetUp()
    {
        dbus.setListener(listener);
        ASSERT_NO_THROW(dbus.availableMaps());
    }

    NXE::DBusController controller;
    NXE::MapDownloaderDBus dbus {controller};
    NXE::MapDownloaderListener listener {
        [this](const std::string &str, std::uint64_t now, std::uint64_t total) {
            nTrace() << "Received progress callback ";
            hasProgressResponse = true;
        },
        [this](const std::string & map, const std::string &err) {
            nTrace() << "An error " << err << " occured";
            hasError = true;
        },
        [this](const std::string & map) {
            nTrace() << "Finished " << map;
            hasFinished = true;
        }
    };

    bool hasProgressResponse {false};
    bool hasError {false};
    bool hasFinished {false};
};

TEST_F(MapDownloaderDBusTest, availableMaps_correct)
{
    auto maps = dbus.availableMaps();
    EXPECT_NE(maps.size(),0);
}

TEST_F(MapDownloaderDBusTest, setOutputPath_incorrect)
{
    EXPECT_FALSE(dbus.setOutputDirectory("/this/is/not/valid/path"));
}

TEST_F(MapDownloaderDBusTest, setOutputPath_correct)
{
    EXPECT_TRUE(dbus.setOutputDirectory("/tmp/maps"));
}

TEST_F(MapDownloaderDBusTest, download_notCorrectMap)
{
    bool val = dbus.download("this country doesn't exists");
    EXPECT_FALSE(val);
}

TEST_F(MapDownloaderDBusTest, download_correctMap)
{
    bool val = dbus.download("Hawaii");
    EXPECT_TRUE(val);

    int maxCounter = 100, counter = 0;

    while(true) {
        std::chrono::milliseconds dura(1000 * 5);
        std::this_thread::sleep_for(dura);
        if (hasError || hasFinished || counter++ > maxCounter) {
            break;
        }
    }
}

TEST_F(MapDownloaderDBusTest, cancel_correct_cancel)
{
    bool val = dbus.download("Picardie");
    EXPECT_TRUE(val);

    // wait 5 seconds
    std::chrono::milliseconds dura(1000 * 10);
    std::this_thread::sleep_for(dura);

    dbus.cancel("Picardie");

    std::chrono::milliseconds dura2(1000);
    std::this_thread::sleep_for(dura2);
    EXPECT_TRUE(hasError);
}
