#ifndef NAVITIPC_H
#define NAVITIPC_H

#include <functional>
#include <boost/signals2/signal.hpp>
#include <map>
#include "position.h"

namespace NXE {

struct Country {
    const std::string name;
    const std::string car;
    const std::string iso2;
    const std::string iso3;
};

struct City {
    const std::string name;
    const std::string postal;
    const std::string postal_mask;
    const std::pair<int,int> position;
};

struct PointClicked {
    typedef std::vector<std::pair<std::string, std::string>> ItemArrayType;
    const Position position;
    const ItemArrayType items;
};

class INavitIPC {
public:

    typedef boost::signals2::signal<void(std::string)> SpeechSignalType;
    typedef boost::signals2::signal<void(const PointClicked&)> PointClickedSignalType;
    typedef boost::signals2::signal<void()> InitializedSignalType;

    virtual ~INavitIPC() {}

    virtual void quit() = 0;

    // Actuall interface description
    virtual void moveBy(int x, int y) = 0;
    virtual int zoom() = 0;
    virtual void zoomBy(int factor) = 0;
    virtual void render() = 0;
    virtual void resize(int x, int y) = 0;

    virtual int orientation() = 0;
    virtual void setOrientation(int newOrientation) = 0;

    virtual void setCenter(double longitude, double latitude) = 0;
    virtual void setDestination(double longitude, double latitude, const std::string& description) = 0;
    virtual void setPosition(double longitude, double latitude) = 0;
    virtual void setPositionByInt(int x, int y) = 0;
    virtual void clearDestination() = 0;
    virtual void addWaypoint(double longitude, double latitude) = 0;

    virtual void setScheme(const std::string& scheme) = 0;

    virtual void startSearch() = 0;
    virtual std::vector<Country> searchCountry(const std::string& what) = 0;
    virtual std::vector<City> searchCity(const std::string& what) = 0;
    virtual void finishSearch() = 0;

    // Signals from IPC
    virtual SpeechSignalType& speechSignal() = 0;
    virtual PointClickedSignalType& pointClickedSignal() = 0;
    virtual InitializedSignalType& initializedSignal() = 0;
};

} // NXE

#endif // NAVITIPC_H
