#include "navitdbus.h"
#include "log.h"
#include "dbuscontroller.h"

#include <thread>
#include <chrono>
#include <map>
#include <dbus-c++/dbus.h>
#include "dbus_helpers.hpp"

#include <boost/signals2/signal.hpp>
#include <boost/format.hpp>

namespace {
const std::string navitDBusDestination = "org.navit_project.navit";
const std::string navitDBusPath = "/org/navit_project/navit/navit/0";
const std::string navitDBusInterface = "org.navit_project.navit.navit";
const std::string rootNavitDBusInterface = "org.navit_project.navit";
const std::string searchNavitDBusInterface = "org.navit_project.navit.search_list";
}

inline DBus::MessageIter& operator>>(::DBus::MessageIter& iter, std::vector<std::pair<std::string, DBus::Variant> >& vec)
{
    if (!iter.is_dict())
        throw DBus::ErrorInvalidArgs("dictionary value expected");
    DBus::MessageIter mit = iter.recurse();

    while (!mit.at_end()) {
        std::string key;
        DBus::Variant value;

        DBus::MessageIter eit = mit.recurse();

        eit >> key >> value;

        vec.push_back(std::make_pair(key, value));

        ++mit;
    }

    return ++iter;
}

struct DebugDBusCall {
    DebugDBusCall(const std::string& msg)
        : _msg(msg)
    {
        nDebug() << "Starting call " << _msg;
    }

    ~DebugDBusCall()
    {
        nDebug() << " Call " << _msg << " took: ";
    }

    const std::string _msg;
};

namespace NXE {

struct NavitDBusObjectProxy : public ::DBus::InterfaceProxy, public ::DBus::ObjectProxy {
    NavitDBusObjectProxy(const std::string& interface, ::DBus::Connection& con)
        : ::DBus::InterfaceProxy(interface)
        , ::DBus::ObjectProxy(con, navitDBusPath, navitDBusDestination.c_str())
    {
        nDebug() << "Connect to signal";
        connect_signal(NavitDBusObjectProxy, signal, signalCallback);
        connect_signal(NavitDBusObjectProxy, startup, startupCallback);
    }

    void signalCallback(const ::DBus::SignalMessage& sig)
    {
        inProgress = true;
        ::DBus::MessageIter it = sig.reader();
        std::vector<std::pair<std::string, DBus::Variant> > res;
        it >> res;

        bool isSpeechSignal = std::find_if(res.begin(), res.end(), [](const std::pair<std::string, DBus::Variant>& val) -> bool {
            if (val.first == "type") {
                const std::string strVal = DBusHelpers::getFromIter<std::string>(val.second.reader());
                return strVal == "speech";
            }
            return false;

        }) != res.end();

        bool isPointClicked = std::find_if(res.begin(), res.end(), [](const std::pair<std::string, ::DBus::Variant>& val) -> bool {
            return val.first == "click_coord_geo";
        }) != res.end();

        if (isSpeechSignal) {
            nDebug() << "Speech callback";
            auto dataIter = std::find_if(res.begin(), res.end(), [](const std::pair<std::string, ::DBus::Variant>& val) -> bool {
                return val.first == "data";
            });

            if (dataIter != res.end()) {
                std::string data = DBusHelpers::getFromIter<std::string>(dataIter->second.reader());
                nDebug() << " I have to say " << data;
                speechSignal(data);
            }
        }
        else if (isPointClicked) {
            nDebug() << "Point callback";
            auto point = unpackPointClicked(res);
            pointClickedSignal(point);
        }

        inProgress = false;
    }

    void startupCallback(const ::DBus::SignalMessage&)
    {
        inProgress = true;
        nTrace() << "Navit has started";
        initializedSignal();
        inProgress = false;
    }

    PointClicked unpackPointClicked(const std::vector<std::pair<std::string, ::DBus::Variant> >& dictionary)
    {
        nDebug() << "Unpacking point";
        double longitude, latitude;
        std::pair<std::string, std::string> oneEntry;
        PointClicked::ItemArrayType items;
        std::for_each(dictionary.begin(), dictionary.end(), [&](const std::pair<std::string, ::DBus::Variant>& p) {
            nDebug() << "Entry= " << p.first;
            if (p.first == "click_coord_geo") {
                std::vector<double> coords = DBusHelpers::getFromIter<std::vector<double>>(p.second.reader());
                longitude = coords.at(0);
                latitude = coords.at(1);
            } else if(p.first == "item_type") {
                oneEntry.first = DBusHelpers::getFromIter<std::string>(p.second.reader());
            } else if(p.first == "label") {
                oneEntry.second = DBusHelpers::getFromIter<std::string>(p.second.reader());
            }

            if (oneEntry.first != "" && oneEntry.second != "") {
                nDebug() << "Adding " << oneEntry.first << " " << oneEntry.second;
                items.push_back(oneEntry);
                oneEntry.first = "";
                oneEntry.second = "";
            }
        });

        PointClicked point{ Position{ latitude, longitude }, items };

        nTrace() << "Clicked item " << point;

        return point;
    }

    INavitIPC::SpeechSignalType speechSignal;
    INavitIPC::PointClickedSignalType pointClickedSignal;
    INavitIPC::InitializedSignalType initializedSignal;
    bool inProgress = false;
};

struct NavitSearchObjectProxy : public ::DBus::InterfaceProxy, public ::DBus::ObjectProxy {
    NavitSearchObjectProxy(const std::string& searchPath, ::DBus::Connection& con)
        : ::DBus::InterfaceProxy(searchNavitDBusInterface)
        , ::DBus::ObjectProxy(con, searchPath, navitDBusDestination.c_str())
    {
    }
};

struct NavitDBusPrivate {
    NavitDBusPrivate(::DBus::Connection& _con)
        : con(_con)
    {
    }

    void createSearchList()
    {
        // create a new search
        ::DBus::Path path{ "/org/navit_project/navit/default_navit/default_mapset" };
        ::DBus::Message retValue = DBusHelpers::call("search_list_new", *(rootObject.get()), path);
        ::DBus::MessageIter iter = retValue.reader();

        ::DBus::Path searchListPath;
        iter >> searchListPath;

        nDebug() << "Search object is " << searchListPath;

        searchObject.reset(new NavitSearchObjectProxy{ searchListPath, con });
    }

    std::shared_ptr<NavitDBusObjectProxy> object;
    std::shared_ptr<NavitDBusObjectProxy> rootObject;
    std::shared_ptr<NavitSearchObjectProxy> searchObject;
    ::DBus::Connection& con;
};

NavitDBus::NavitDBus(DBusController& ctrl)
    : d(new NavitDBusPrivate{ ctrl.connection() })
{
    nTrace() << "NavitDBus::NavitDBus()";
    d->object.reset(new NavitDBusObjectProxy(navitDBusInterface, ctrl.connection()));
    d->rootObject.reset(new NavitDBusObjectProxy(rootNavitDBusInterface, ctrl.connection()));
}

NavitDBus::~NavitDBus()
{
    nTrace() << "Destroying navit dbus";
}

void NavitDBus::quit()
{
    DebugDBusCall db{ "quit" };
    DBusHelpers::call("quit", *(d->object.get()));
}

void NavitDBus::moveBy(int x, int y)
{
    DBus::CallMessage call;
    DBus::MessageIter it = call.writer();
    call.member("set_center_screen");
    DBus::Struct<int, int> val;
    val._1 = x;
    val._2 = y;
    DBusHelpers::call("set_center_screen", *(d->object.get()), val);
}

void NavitDBus::zoomBy(int y)
{
    nDebug() << "Zooming by " << y;
    DBusHelpers::call("zoom", *(d->object.get()), y);
}

int NavitDBus::zoom()
{
    nDebug() << "Getting zoom";
    return DBusHelpers::getAttr<int>("zoom", *(d->object.get()));
}

void NavitDBus::render()
{
    nDebug() << "Rendering";
    DBusHelpers::call("draw", *(d->object.get()));
    nDebug() << "Rendering finished";
}

void NavitDBus::resize(int x, int y)
{
    nDebug() << "Resizing [" << x << "x" << y << "]";
    DBusHelpers::call("resize", *(d->object.get()), x, y);
}

int NavitDBus::orientation()
{
    return DBusHelpers::getAttr<int>("orientation", *(d->object.get()));
}

void NavitDBus::setOrientation(int newOrientation)
{
    nDebug() << "Changing orientation to " << newOrientation;
    if (newOrientation != 0 && newOrientation != -1) {
        nError() << "Unable to change orientation to " << newOrientation;
        throw std::runtime_error("Unable to change orientation. Incorrect value, value can only be -1/0");
    }
    DBusHelpers::setAttr("orientation", *(d->object.get()), newOrientation);
}

void NavitDBus::setCenter(double longitude, double latitude)
{
    auto format = boost::format("geo: %1% %2%") % longitude % latitude;
    const std::string message = format.str();

    DBusHelpers::call("set_center_by_string", *(d->object.get()), message);
}

void NavitDBus::setDestination(double longitude, double latitude, const std::string& description)
{
    nDebug() << "Setting destionation to. name= " << description;
    auto format = boost::format("geo: %1% %2%") % longitude % latitude;
    const std::string message = format.str();

    DBusHelpers::call("set_destination", *(d->object.get()), message, description);
}

void NavitDBus::setPosition(double longitude, double latitude)
{
    auto format = boost::format("geo: %1% %2%") % longitude % latitude;
    const std::string message = format.str();

    DBusHelpers::call("set_position", *(d->object.get()), message);
}

void NavitDBus::setPositionByInt(int x, int y)
{
    DBus::Struct<int, int, int> s;
    s._1 = 1;
    s._2 = x;
    s._3 = y;
    DBusHelpers::call("set_center", *(d->object.get()), s);
}

void NavitDBus::addWaypoint(double longitude, double latitude)
{
    auto format = boost::format("geo: %1% %2%") % longitude % latitude;
    const std::string message = format.str();

    DBusHelpers::call("add_waypoint", *(d->object.get()), message);
}

void NavitDBus::clearDestination()
{
    DBusHelpers::call("clear_destination", *(d->object.get()));
}

void NavitDBus::setScheme(const std::string& scheme)
{
    DebugDBusCall db{ "set_layout" };
    nDebug() << "Setting scheme to " << scheme;
    DBusHelpers::call("set_layout", *(d->object.get()), scheme);
}

void NavitDBus::startSearch()
{
    d->createSearchList();
}

std::vector<Country> NavitDBus::searchCountry(const std::string& countryName)
{
    if (!d->searchObject)
        throw std::runtime_error("startSearch not called");
    nTrace() << "Searching for " << countryName;
    ::DBus::Variant var;
    ::DBus::MessageIter variantIter = var.writer();
    variantIter << countryName;

    DBusHelpers::call("search", *(d->searchObject.get()), std::string{ "country_name" }, var, static_cast<int>(1));
    std::vector<Country> ret;

    while (true) {
        try {
            ::DBus::Message results = DBusHelpers::call("get_result", *(d->searchObject.get()));
            ::DBus::MessageIter resultsIter{ results.reader() };

            std::int32_t resultId;
            ::DBus::Struct<int, int, int> position;
            typedef std::map<std::string, std::map<std::string, ::DBus::Variant> > LocationDBusType;
            LocationDBusType at;
            Position ctryPos;
            ctryPos.latitude = position._1;

            resultsIter >> resultId >> position;
            resultsIter >> at;

            nDebug() << "Search results: " << resultId << " Pos = "
                     << position._1 << " " << position._2 << " " << position._3;

            ret.emplace_back(Country{
                DBusHelpers::getFromIter<std::string>(at.at("country").at("name").reader()),
                DBusHelpers::getFromIter<std::string>(at.at("country").at("car").reader()),
                DBusHelpers::getFromIter<std::string>(at.at("country").at("iso2").reader()),
                DBusHelpers::getFromIter<std::string>(at.at("country").at("iso3").reader()) });
        }
        catch (const std::exception& ex) {
            break;
        }
    }

    return ret;
}

std::vector<City> NavitDBus::searchCity(const std::string& cityName)
{
    if (!d->searchObject)
        throw std::runtime_error("startSearch not called");
    nTrace() << "Searching for " << cityName;
    ::DBus::Variant var;
    ::DBus::MessageIter variantIter = var.writer();
    variantIter << cityName;

    DBusHelpers::call("search", *(d->searchObject.get()), std::string{ "town_name" }, var, static_cast<int>(1));
    std::vector<City> cities;
    while (true) {
        try {
            ::DBus::Message results = DBusHelpers::call("get_result", *(d->searchObject.get()));
            ::DBus::MessageIter resultsIter{ results.reader() };

            std::int32_t resultId;
            ::DBus::Struct<int, int, int> position;
            typedef std::map<std::string, std::map<std::string, ::DBus::Variant> > LocationDBusType;
            LocationDBusType at;
            int o, x, y;

            resultsIter >> resultId >> position;
            resultsIter >> at;
            o = position._1;
            x = position._2;
            y = position._3;

            nDebug() << "Search results: " << resultId << " Pos = "
                     << position._1 << " " << position._2 << " " << position._3;

            cities.emplace_back(City{
                DBusHelpers::getFromIter<std::string>(at.at("town").at("name").reader()),
                DBusHelpers::getFromIter<std::string>(at.at("town").at("postal").reader()),
                DBusHelpers::getFromIter<std::string>(at.at("town").at("postal_mask").reader()),
                std::make_pair(x, y)
            });
            nDebug() << "[Name] = " << cities.back().name << " [postal] = " << cities.back().postal;
        }
        catch (const std::exception& ex) {
            break;
        }
    }
    return cities;
}

void NavitDBus::finishSearch()
{
    nInfo() << "Destroying search list";
    DBusHelpers::call("destroy", *(d->searchObject.get()));
}

INavitIPC::SpeechSignalType& NavitDBus::speechSignal()
{
    assert(d && d->object);
    return d->object->speechSignal;
}

INavitIPC::PointClickedSignalType& NavitDBus::pointClickedSignal()
{
    assert(d && d->object);
    return d->object->pointClickedSignal;
}

INavitIPC::InitializedSignalType& NavitDBus::initializedSignal()
{
    return d->object->initializedSignal;
}

} // namespace NXE
