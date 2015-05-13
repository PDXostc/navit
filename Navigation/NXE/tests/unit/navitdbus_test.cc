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
    ASSERT_NE(country.size(), 0);
    EXPECT_EQ(country.at(0).country.name, "Germany");
    EXPECT_EQ(country.at(0).country.car, "D");
    EXPECT_EQ(country.at(0).country.iso2, "DE");
    EXPECT_EQ(country.at(0).country.iso3, "DEU");

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
    auto cities = connection.search(NXE::INavitIPC::SearchType::City, "Berlin");
    ASSERT_NE(cities.size(), 0);
    //Country checks
    EXPECT_EQ(cities.at(0).country.name, "Germany");
    EXPECT_EQ(cities.at(0).country.car, "D");
    EXPECT_EQ(cities.at(0).country.iso2, "DE");
    EXPECT_EQ(cities.at(0).country.iso3, "DEU");
    EXPECT_EQ(cities.at(0).country.name, "Germany");
    auto it = std::find_if(cities.begin(), cities.end(), [](const NXE::SearchResult& city) -> bool {
        return city.city.name == "Berlin";
    });
    ASSERT_NE(it, cities.end());

    EXPECT_EQ(it->city.name, "Berlin");
    EXPECT_EQ(it->country.name, "Germany");
    connection.finishSearch();
}

TEST_F(NavitDBusTest, search_street_valid)
{
    connection.startSearch();
    auto country = connection.search(NXE::INavitIPC::SearchType::Country, "Germany");
    auto cities = connection.search(NXE::INavitIPC::SearchType::City, "Berlin");
    auto streets = connection.search(NXE::INavitIPC::SearchType::Street, "Benfeyweg");
    ASSERT_NE(country.size(), 0);
    ASSERT_NE(cities.size(), 0);
    EXPECT_TRUE(std::find_if(streets.begin(), streets.end(), [](const NXE::SearchResult& street) -> bool {
        return street.street.name == "Benfeyweg";
    }) != streets.end());

    connection.finishSearch();
}

TEST_F(NavitDBusTest, search_address_valid)
{
    connection.startSearch();

    auto country = connection.search(NXE::INavitIPC::SearchType::Country, "Germany");
    ASSERT_GT(country.size(), 0);
    connection.selectSearchResult(NXE::INavitIPC::SearchType::Country, country.at(0).searchId);

    auto cities = connection.search(NXE::INavitIPC::SearchType::City, "Berlin");
    ASSERT_GT(cities.size(), 2);
    connection.selectSearchResult(NXE::INavitIPC::SearchType::City, cities.at(1).searchId);

    auto streets = connection.search(NXE::INavitIPC::SearchType::Street, "Benfeyweg");
    ASSERT_EQ(streets.size(),1);
    connection.selectSearchResult(NXE::INavitIPC::SearchType::Street, streets.at(0).searchId);

    auto addresses = connection.search(NXE::INavitIPC::SearchType::Address, "4");
    ASSERT_NE(country.size(), 0);
    ASSERT_NE(cities.size(), 0);
    ASSERT_TRUE(std::find_if(streets.begin(), streets.end(), [](const NXE::SearchResult& street) -> bool {
        return street.street.name == "Benfeyweg";
    }) != streets.end());

    EXPECT_NE(addresses.size(), 0);

    connection.finishSearch();
}

TEST_F(NavitDBusTest, search_incomplete_address_valid)
{
    connection.startSearch();

    auto country = connection.search(NXE::INavitIPC::SearchType::Country, "Germany");
    ASSERT_GT(country.size(), 0);
    connection.selectSearchResult(NXE::INavitIPC::SearchType::Country, country.at(0).searchId);

    auto cities = connection.search(NXE::INavitIPC::SearchType::City, "Berlin");
    ASSERT_GT(cities.size(), 2);
    connection.selectSearchResult(NXE::INavitIPC::SearchType::City, cities.at(1).searchId);

    auto streets = connection.search(NXE::INavitIPC::SearchType::Street, "Benf");
    ASSERT_EQ(streets.size(),2);
    connection.selectSearchResult(NXE::INavitIPC::SearchType::Street, streets.at(1).searchId);

    auto addresses = connection.search(NXE::INavitIPC::SearchType::Address, "");
    ASSERT_NE(country.size(), 0);
    ASSERT_NE(cities.size(), 0);
    ASSERT_TRUE(std::find_if(streets.begin(), streets.end(), [](const NXE::SearchResult& street) -> bool {
        return street.street.name == "Benfeyweg";
    }) != streets.end());

    EXPECT_NE(addresses.size(), 0);

    connection.finishSearch();
}
