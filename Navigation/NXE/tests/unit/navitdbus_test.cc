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

bool waitFor(bool &forWhat, int numberOfTimeouts = 5) {
    const int defCounter = numberOfTimeouts;
    int counter = 0;
    bool ret = false;
    while(true) {
        std::chrono::milliseconds dura(50);
        std::this_thread::sleep_for(dura);

        if (forWhat) {
            ret = true;
            break;
        }

        if (counter++ > defCounter) {
            break;
        }
    }
    return ret;
}

struct NavitDBusTest : public ::testing::Test {

    NXE::DBusController controller;
    NXE::NavitDBus connection{ controller };
    NXE::NavitProcessImpl process;
    bool rec {false};
    NXE::SearchResults results;

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

        connection.searchResponse().connect(std::bind(&NavitDBusTest::recFunc, this, std::placeholders::_1));
    }

    void TearDown()
    {
        connection.quit();
        controller.stop();
        if (runNavit) {
            process.stop();
        }
    }

    void recFunc(NXE::SearchResults res)
    {
        nTrace() << "here";
        rec = true;
        results = res;
    }

    NXE::SearchResults searchAndBlock(NXE::INavitIPC::SearchType t, const std::string& s) {
        rec = false;
        nTrace() << "Start searching " << rec;
        connection.search(t,s);
        nTrace() << "Wait for " << rec;
        bool bw = waitFor(rec, 20);
        nTrace() << "Already done?" << rec << " " << bw;
        return results;
    }
};

TEST_F(NavitDBusTest, zoom)
{
    bool received{ false };
    int defZoom;
    connection.zoomResponse().connect([&received, &defZoom](int zoom) {
        received = true;
        defZoom = zoom;
    });
    connection.zoom();

    ASSERT_TRUE(waitFor(received));
    nDebug()<< "Zoom is " << defZoom;
    auto oldZoom = defZoom;
    received = false;

    connection.setZoom(oldZoom/2);
    connection.zoom();
    ASSERT_TRUE(waitFor(received));
    auto newZoom = defZoom;

    nDebug() << "Zooms " << oldZoom << " " << newZoom;

    EXPECT_NE(newZoom, oldZoom);
}

TEST_F(NavitDBusTest, setZoom)
{
    bool received{ false };
    int defZoom = 0;
    connection.zoomResponse().connect([&received, &defZoom](int zoom) {
        received = true;
        defZoom = zoom;
    });
    connection.setZoom(16);
    connection.zoom();

    ASSERT_TRUE(waitFor(received));

    EXPECT_EQ(16, defZoom);
}

TEST_F(NavitDBusTest, setOrientation)
{
    bool received {false};
    int orientation;
    connection.orientationResponse().connect([&](int newOrientation){
        received = true;
        orientation = newOrientation;
    });

    connection.orientation();

    ASSERT_TRUE(waitFor(received));
    EXPECT_ANY_THROW(connection.setOrientation(1));

    auto oldOrientation = orientation;
    if (orientation == 0) {
        connection.setOrientation(-1);
    }
    else {
        connection.setOrientation(0);
    }

    connection.orientation();
    ASSERT_TRUE(waitFor(received));
    EXPECT_NE(orientation, oldOrientation);
}

TEST_F(NavitDBusTest, setCenter)
{
    connection.setCenter(24.0, 53.0);
}

TEST_F(NavitDBusTest, setPosition)
{
    connection.setPosition(24.0, 53.0);
}

TEST_F(NavitDBusTest, setDestination)
{
    connection.setDestination(24.10, 53.10, "dest1");
}

TEST_F(NavitDBusTest, clearDestination)
{
    connection.clearDestination();
}

TEST_F(NavitDBusTest, addWaypoint)
{
    connection.addWaypoint(11.586, 48.149);
}

TEST_F(NavitDBusTest, SearchPois)
{
    bool received {false};
    connection.searchPoiResponse().connect([&] (){ received = true;});
    connection.searchPOIs(11.5659, 48.1392, 500);
    waitFor(received);
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
    auto results = searchAndBlock(NXE::INavitIPC::SearchType::Country, "Germany");
    ASSERT_NE(results.size(), 0);
    EXPECT_EQ(results.at(0).country.name, "Germany");
    EXPECT_EQ(results.at(0).country.car, "D");
    EXPECT_EQ(results.at(0).country.iso2, "DE");
    EXPECT_EQ(results.at(0).country.iso3, "DEU");

    connection.finishSearch();
}

TEST_F(NavitDBusTest, search_multipleCountries)
{
    connection.startSearch();
    auto results = searchAndBlock(NXE::INavitIPC::SearchType::Country, "P");
    EXPECT_NE(results.size(), 0);
    connection.finishSearch();
}

TEST_F(NavitDBusTest, search_city_valid)
{
    connection.startSearch();
    searchAndBlock(NXE::INavitIPC::SearchType::Country, "Germany");
    auto cities = searchAndBlock(NXE::INavitIPC::SearchType::City, "Berlin");
    //Country checks
    ASSERT_NE(cities.size(), 0);
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
    auto country = searchAndBlock(NXE::INavitIPC::SearchType::Country, "Germany");
    auto city = searchAndBlock(NXE::INavitIPC::SearchType::City, "Berlin");
    auto streets = searchAndBlock(NXE::INavitIPC::SearchType::Street, "Benfeyweg");
    ASSERT_NE(country.size(), 0);
    ASSERT_NE(city.size(), 0);
    EXPECT_TRUE(std::find_if(streets.begin(), streets.end(), [](const NXE::SearchResult& street) -> bool {
        return street.street.name == "Benfeyweg";
    }) != streets.end());

    connection.finishSearch();
}

TEST_F(NavitDBusTest, search_address_valid)
{
    connection.startSearch();

    auto country = searchAndBlock(NXE::INavitIPC::SearchType::Country, "Germany");
    ASSERT_GT(country.size(), 0);
    connection.selectSearchResult(NXE::INavitIPC::SearchType::Country, country.at(0).searchId);

    auto cities = searchAndBlock(NXE::INavitIPC::SearchType::City, "Berlin");
    ASSERT_GT(cities.size(), 2);
    connection.selectSearchResult(NXE::INavitIPC::SearchType::City, cities.at(1).searchId);

    auto streets = searchAndBlock(NXE::INavitIPC::SearchType::Street, "Benfeyweg");
    ASSERT_EQ(streets.size(), 1);
    connection.selectSearchResult(NXE::INavitIPC::SearchType::Street, streets.at(0).searchId);

    auto addresses = searchAndBlock(NXE::INavitIPC::SearchType::Address, "4");
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

    auto country = searchAndBlock(NXE::INavitIPC::SearchType::Country, "Germany");
    ASSERT_GT(country.size(), 0);
    connection.selectSearchResult(NXE::INavitIPC::SearchType::Country, country.at(0).searchId);

    auto cities = searchAndBlock(NXE::INavitIPC::SearchType::City, "Berlin");
    ASSERT_GT(cities.size(), 2);
    connection.selectSearchResult(NXE::INavitIPC::SearchType::City, cities.at(1).searchId);

    auto streets = searchAndBlock(NXE::INavitIPC::SearchType::Street, "Benf");
    ASSERT_EQ(streets.size(), 2);
    connection.selectSearchResult(NXE::INavitIPC::SearchType::Street, streets.at(1).searchId);

    auto addresses = searchAndBlock(NXE::INavitIPC::SearchType::Address, "");
    ASSERT_NE(country.size(), 0);
    ASSERT_NE(cities.size(), 0);
    ASSERT_TRUE(std::find_if(streets.begin(), streets.end(), [](const NXE::SearchResult& street) -> bool {
        return street.street.name == "Benfeyweg";
    }) != streets.end());

    EXPECT_NE(addresses.size(), 0);

    connection.finishSearch();
}

TEST_F(NavitDBusTest, currentCenter)
{
    NXE::Position pos;
    bool rec {false};
    connection.currentCenterResponse().connect([&](NXE::Position p) {
        rec = true;
        pos = p;

    });
    connection.currentCenter();
    ASSERT_TRUE(waitFor(rec));
    EXPECT_NE(pos.latitude, 0);
    EXPECT_NE(pos.latitude, 0);
}

TEST_F(NavitDBusTest, changeCenter)
{
    NXE::Position pos;
    bool rec {false};
    connection.currentCenterResponse().connect([&](NXE::Position p) {
        nDebug()<< "center received";
        rec = true;
        pos = p;
    });
    connection.currentCenter();
    ASSERT_TRUE(waitFor(rec));
    auto lastCenter = pos;
    connection.setCenter(lastCenter.longitude + 12.0,
        lastCenter.latitude - 27);
    connection.currentCenter();
    ASSERT_TRUE(waitFor(rec));
    auto lastCenter2 = pos;

    EXPECT_NE(lastCenter.longitude, lastCenter2.longitude);

    waitFor(rec);
    connection.quit();
    std::chrono::milliseconds dura(1000);
    std::this_thread::sleep_for(dura);
    process.stop();
    process.start();
    std::this_thread::sleep_for(dura);

    nDebug() << "gimme center";
    connection.currentCenter();
    ASSERT_TRUE(waitFor(rec, 20));

    EXPECT_DOUBLE_EQ(lastCenter2.latitude, pos.latitude);
}
