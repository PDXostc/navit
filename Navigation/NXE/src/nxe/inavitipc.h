#ifndef NAVITIPC_H
#define NAVITIPC_H

#include <functional>
#include <boost/signals2/signal.hpp>
#include <map>
#include "position.h"

namespace NXE {

struct SearchResult {
    int32_t searchId;
    std::pair<double, double> position;
    // Country
    struct Country {
        std::string name;
        std::string car;
        std::string iso2;
        std::string iso3;
    };
    struct City {
        std::string name;
        std::string postal;
        std::string postal_mask;
    };
    struct Street {
        std::string name;
    };
    struct HouseNumber {
        std::string name;
        std::string postal;
        std::string postal_mask;
    };

    Country country;
    City city;
    Street street;
    HouseNumber house;
};

typedef std::vector<SearchResult> SearchResults;

struct PointClicked {
    typedef std::vector<std::pair<std::string, std::string>> ItemArrayType;
    const Position position;
    const ItemArrayType items;
};

class INavitIPC {
public:

    enum class SearchType {
        Country = 0,
        City,
        Street,
        Address
    };

    typedef boost::signals2::signal<void(std::string)> SpeechSignalType;
    typedef boost::signals2::signal<void(std::string)> RoutingSignalType;
    typedef boost::signals2::signal<void(const PointClicked&)> PointClickedSignalType;
    typedef boost::signals2::signal<void()> InitializedSignalType;

    // dbus get
    typedef boost::signals2::signal<void(std::int32_t)> IntSignalType;
    typedef boost::signals2::signal<void(bool)> BoolSignalType;
    typedef boost::signals2::signal<void()> EmptySignalType;
    typedef boost::signals2::signal<void(NXE::Position)> CurrentCenterSignalType;
    typedef boost::signals2::signal<void(SearchResults, SearchType)> SearchResultsSignalType;

    virtual ~INavitIPC() {}

    virtual void quit() = 0;

    // Actuall interface description
    virtual void zoom() = 0;
    virtual void setZoom(int newZoom) = 0;
    virtual void render() = 0;
    virtual void resize(int x, int y) = 0;

    virtual void orientation() = 0;
    virtual void setOrientation(int newOrientation) = 0;

    virtual void setCenter(double longitude, double latitude) = 0;
    virtual void setDestination(double longitude, double latitude, const std::string& description) = 0;
    virtual bool isNavigationRunning() = 0;
    virtual void setPosition(double longitude, double latitude) = 0;
    virtual void clearDestination() = 0;
    virtual void addWaypoint(double longitude, double latitude) = 0;
    virtual void searchPOIs(double longitude, double latitude, int distance) = 0;
    virtual void setPitch(std::uint16_t newPitchValue) = 0;

    virtual void setScheme(const std::string& scheme) = 0;
    virtual void currentCenter() = 0;

    virtual void startSearch() = 0;
    virtual void search(SearchType type, const std::string& searchString) = 0;
    virtual void selectSearchResult(SearchType type, std::int32_t id)  = 0;
    virtual void finishSearch() = 0;
    virtual void setTracking(bool tracking) = 0;

    virtual void distance() = 0;
    virtual void eta() = 0;
    // DBus responses
    virtual IntSignalType& orientationResponse() = 0;
    virtual IntSignalType& zoomResponse() = 0;
    virtual EmptySignalType& searchPoiResponse() = 0;
    virtual CurrentCenterSignalType& currentCenterResponse() = 0;
    virtual SearchResultsSignalType& searchResponse() = 0;
    virtual IntSignalType& distanceResponse() = 0;
    virtual IntSignalType& etaResponse() = 0;
    virtual BoolSignalType& navigationChanged() = 0;


    // Signals from IPC
    virtual SpeechSignalType& speechSignal() = 0;
    virtual PointClickedSignalType& pointClickedSignal() = 0;
    virtual InitializedSignalType& initializedSignal() = 0;
    virtual RoutingSignalType& routingSignal() = 0;
};



} // NXE

#endif // NAVITIPC_H
