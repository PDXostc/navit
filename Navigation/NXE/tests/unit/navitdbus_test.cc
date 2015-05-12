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
        std::chrono::milliseconds dura(1000);
        std::this_thread::sleep_for(dura);
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

TEST_F(NavitDBusTest, setZoom)
{
    EXPECT_NO_THROW(
        connection.setZoom(16);
        EXPECT_EQ(16, connection.zoom());
    );
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

TEST_F(NavitDBusTest, setPositionByInt)
{
    connection.setPositionByInt(0x138a4a, 0x5d773f);
}

TEST_F(NavitDBusTest, setDestination)
{
    connection.setDestination(24.10,53.10,"dest1");
}

TEST_F(NavitDBusTest, clearDestination)
{
    connection.clearDestination();
}

TEST_F(NavitDBusTest, addWaypoint)
{
    connection.addWaypoint(11.586, 48.149);
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

TEST_F(NavitDBusTest, search_country)
{
    connection.startSearch();
    auto country = connection.search(NXE::INavitIPC::SearchType::Country, "Germany");
    EXPECT_NE(country.size(), 0);
    connection.finishSearch();
}

TEST_F(NavitDBusTest, search_multipleCountries)
{
    connection.startSearch();
    auto country = connection.search(NXE::INavitIPC::SearchType::Country, "P");
    EXPECT_NE(country.size(), 0);
    connection.finishSearch();
}

TEST_F(NavitDBusTest, search_street_name_invalid)
{
    EXPECT_ANY_THROW(
        connection.search(NXE::INavitIPC::SearchType::Country, "");
    );
}

TEST_F(NavitDBusTest, search_city_valid)
{
    connection.startSearch();
    auto country = connection.search(NXE::INavitIPC::SearchType::Country, "Germany");
    auto cities = connection.search(NXE::INavitIPC::SearchType::City, "Mun");
    ASSERT_NE(country.size(), 0);
    EXPECT_EQ(country.at(0).name, "Germany");
    ASSERT_NE(cities.size(), 0);
    EXPECT_TRUE(std::find_if(cities.begin(), cities.end(), [](const NXE::SearchResult& city) -> bool {
        return city.name == "München";
    }) != cities.end());
    connection.finishSearch();
}

TEST_F(NavitDBusTest, search_street_valid)
{
    connection.startSearch();
    auto country = connection.search(NXE::INavitIPC::SearchType::Country, "Germany");
    auto cities = connection.search(NXE::INavitIPC::SearchType::City, "Mun");
    auto streets = connection.search(NXE::INavitIPC::SearchType::Street, "Arc");
    ASSERT_NE(country.size(), 0);
    ASSERT_NE(cities.size(), 0);
    EXPECT_TRUE(std::find_if(streets.begin(), streets.end(), [](const NXE::SearchResult& street) -> bool {
        return street.name == "Arcisstraße";
    }) != streets.end());

    connection.finishSearch();
}

TEST_F(NavitDBusTest, search_address_valid)
{
    connection.startSearch();
    auto country = connection.search(NXE::INavitIPC::SearchType::Country, "Germany");
    auto cities = connection.search(NXE::INavitIPC::SearchType::City, "Mun");
    auto streets = connection.search(NXE::INavitIPC::SearchType::Street, "Arc");
    auto addresses = connection.search(NXE::INavitIPC::SearchType::Address, "1");

    ASSERT_NE(country.size(), 0);
    ASSERT_NE(cities.size(), 0);
    EXPECT_TRUE(std::find_if(streets.begin(), streets.end(), [](const NXE::SearchResult& street) -> bool {
        return street.name == "Arcisstraße";
    }) != streets.end());

    connection.finishSearch();
}
