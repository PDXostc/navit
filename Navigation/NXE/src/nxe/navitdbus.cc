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

std::string convert(NXE::INavitIPC::SearchType type)
{
    std::string tag;
    if (type == NXE::INavitIPC::SearchType::Country) {
        tag = "country_all";
    }
    else if (type == NXE::INavitIPC::SearchType::City) {
        tag = "town_name";
    }
    else if (type == NXE::INavitIPC::SearchType::Street) {
        tag = "street_name";
    }
    else if (type == NXE::INavitIPC::SearchType::Address) {
        tag = "house_number";
    }
    return tag;
}
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

template <typename TimeT = std::chrono::milliseconds>
struct measure {
    template <typename F, typename... Args>
    static typename TimeT::rep execution(F func, Args&&... args)
    {
        auto start = std::chrono::system_clock::now();
        func(std::forward<Args>(args)...);
        auto duration = std::chrono::duration_cast<TimeT>(std::chrono::system_clock::now() - start);
        return duration.count();
    }
};

struct DebugDBusCall {
    typedef std::chrono::high_resolution_clock ClockType;
    DebugDBusCall(const std::string& msg)
        : _msg(msg)
    {
        nDebug() << "Starting call " << _msg;
        _start = ClockType::now();
    }

    ~DebugDBusCall()
    {
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(ClockType::now() - _start);
        nDebug() << " Call " << _msg << " took: " << milliseconds.count();
    }

    ClockType::time_point _start;
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
    DBus::Connection& con;
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
    DBus::Struct<int, int> val;
    val._1 = x;
    val._2 = y;
    DBusHelpers::call("set_center_screen", *(d->object.get()), val);
}

void NavitDBus::zoomBy(int y)
{
    nDebug() << "Zooming by " << y;
    DBusHelpers::callNoReply("zoom", *(d->object.get()), y);
}

void NavitDBus::setZoom(int newZoom)
{
    nDebug() << "Setting zoom = " << newZoom;
    DBusHelpers::setAttr("zoom", *(d->object.get()), newZoom);
}

int NavitDBus::zoom()
{
    nDebug() << "Getting zoom";
    return DBusHelpers::getAttr<int>("zoom", *(d->object.get()));
}

void NavitDBus::render()
{
    DebugDBusCall dbg{ "draw" };
    DBusHelpers::callNoReply("draw", *(d->object.get()));
    nDebug() << "Rendering finished";
}

void NavitDBus::resize(int x, int y)
{
    DebugDBusCall dbg{ "resize" };
    nDebug() << "Resizing [" << x << "x" << y << "]";
    DBusHelpers::callNoReply("resize", *(d->object.get()), x, y);
}

int NavitDBus::orientation()
{
    return DBusHelpers::getAttr<int>("orientation", *(d->object.get()));
}

void NavitDBus::setOrientation(int newOrientation)
{
    DebugDBusCall dbg{ "set_attr_orientation" };
    nDebug() << "Changing orientation to " << newOrientation;
    if (newOrientation != 0 && newOrientation != -1) {
        nError() << "Unable to change orientation to " << newOrientation;
        throw std::runtime_error("Unable to change orientation. Incorrect value, value can only be -1/0");
    }
    DBusHelpers::setAttr("orientation", *(d->object.get()), newOrientation);
}

void NavitDBus::setCenter(double longitude, double latitude)
{
    DebugDBusCall dbg{ "set_center_by_string" };
    auto format = boost::format("geo: %1% %2%") % longitude % latitude;
    const std::string message = format.str();

    DBusHelpers::callNoReply("set_center_by_string", *(d->object.get()), message);
}

void NavitDBus::setDestination(double longitude, double latitude, const std::string& description)
{
    DebugDBusCall dbg{ "setDestination" };
    nDebug() << "Setting destionation to. name= " << description;
    auto format = boost::format("geo: %1% %2%") % longitude % latitude;
    const std::string message = format.str();

    DBusHelpers::call("set_destination", *(d->object.get()), message, description);
}

void NavitDBus::setPosition(double longitude, double latitude)
{
    DebugDBusCall dbg{ "setPosition" };
    auto format = boost::format("geo: %1% %2%") % longitude % latitude;
    const std::string message = format.str();
    DBus::Struct<int, std::string> s;
    s._1 = 1;
    s._2 = message;

    DBusHelpers::call("set_center", *(d->object.get()),s);
}

void NavitDBus::setPositionByInt(int x, int y)
{
    DebugDBusCall dbg{ "setPositionByInt" };
    DBus::Struct<int, int, int> s;
    s._1 = 1;
    s._2 = x;
    s._3 = y;
    DBusHelpers::callNoReply("set_center", *(d->object.get()), s);
}

void NavitDBus::addWaypoint(double longitude, double latitude)
{
    DebugDBusCall db{ "add_waypoint" };
    auto format = boost::format("geo: %1% %2%") % longitude % latitude;
    const std::string message = format.str();

    DBusHelpers::call("add_waypoint", *(d->object.get()), message);
}

void NavitDBus::clearDestination()
{
    DebugDBusCall db{ "clear_destination" };
    DBusHelpers::call("clear_destination", *(d->object.get()));
}

void NavitDBus::setScheme(const std::string& scheme)
{
    DebugDBusCall db{ "set_layout" };
    nDebug() << "Setting scheme to " << scheme;
    DBusHelpers::callNoReply("set_layout", *(d->object.get()), scheme);
}

void NavitDBus::setPitch(std::uint16_t newPitchValue)
{
    std::int32_t pitchVal = static_cast<std::int32_t>(newPitchValue);
    nDebug() << "Setting pitch to = " << pitchVal;
    DBusHelpers::setAttr("pitch", *(d->object.get()), pitchVal);
}

void NavitDBus::searchPOIs(double longitude, double latitude, int dist)
{
    DebugDBusCall dbg{ "searchPOIs" };
    nDebug() << "searchPOIs in " << dist << " distance";
    auto format = boost::format("geo: %1% %2%") % longitude % latitude;
    auto format1 = boost::format("%1%") % dist;
    const std::string center_coord = format.str();
    const std::string distance = format1.str();

    DBusHelpers::call("search_pois", *(d->object.get()), center_coord, distance);
}
Position NavitDBus::currentCenter()
{
    nInfo() << "Current center";
    auto ret = DBusHelpers::getAttr<DBus::Struct<double,double>>("center", *(d->object.get()));
    nInfo() << "Current center " << ret._1 << ret._2;

    return NXE::Position{ret._1, ret._2};
}

void NavitDBus::startSearch()
{
    nInfo() << "Creating new search";
    DebugDBusCall db{ "create_search" };
    d->createSearchList();
}

SearchResults NavitDBus::search(INavitIPC::SearchType type, const std::string& searchString)
{
    if (!d->searchObject)
        throw std::runtime_error("startSearch not called");

    DebugDBusCall db{ "search" };
    nTrace() << "Searching for " << searchString;
    std::string tag = convert(type);

    nTrace() << "Tag =" << tag;
    SearchResults ret;

    DBus::Variant var;
    DBus::MessageIter variantIter = var.writer();
    variantIter << searchString;

    DBusHelpers::call("search", *(d->searchObject.get()), tag, var, static_cast<int>(1));
    while (true) {
        try {
            DBus::Message results = DBusHelpers::call("get_result", *(d->searchObject.get()));
            nTrace() << "Received result";
            DBus::MessageIter resultsIter{ results.reader() };

            std::int32_t resultId;
            DBus::Struct<double,double> position;
            typedef std::map<std::string, std::map<std::string, ::DBus::Variant> > LocationDBusType;
            LocationDBusType at;
            double lat, lon;

            resultsIter >> resultId >> position;
            nTrace() << "Read pos";
            resultsIter >> at;
            lat = position._2;
            lon = position._1;

            nDebug() << "Search results: " << resultId << " Pos = "
                     << position._1 << " " << position._2;

            auto decodeCountry = [this](LocationDBusType& map) -> SearchResult::Country {
                std::string car,name = map.at("country").at("name");
                auto isCar = map.at("country").find("car");
                if (isCar != map.at("country").end()) {
                    auto reader = isCar->second.reader();
                    reader >> car;
                }
                return SearchResult::Country{name,
                            car,
                            map.at("country")["iso2"],
                            map.at("country")["iso3"]
                };
            };
            auto decodeCity = [this](LocationDBusType& map) -> SearchResult::City {
                std::string postal, postalMask;
                auto isPostal = map.at("town").find("postal");
                if (isPostal != map.at("town").end()) {
                    postal = DBusHelpers::getFromIter<std::string>(isPostal->second.reader());
                }
                auto isPostalMask = map.at("town").find("postal_mask");
                if (isPostalMask != map.at("town").end()) {
                    postal = DBusHelpers::getFromIter<std::string>(isPostalMask->second.reader());
                }
                return SearchResult::City{
                    map.at("town").at("name"),
                            postal,
                            postalMask
                };
            };
            auto decodeStreet = [this](const LocationDBusType& map) -> SearchResult::Street {
                std::string name;
                auto isStreet = map.find("street");
                if (isStreet != map.end()) {
                    name = DBusHelpers::getFromIter<std::string>(isStreet->second.at("name").reader());

                }
                return SearchResult::Street{
                    name
                };
            };
            auto decodeHouse = [this](LocationDBusType& map) -> SearchResult::HouseNumber {
                return SearchResult::HouseNumber{
                    map.at("housenumber").at("name"),
                    map.at("housenumber")["postal"],
                    map.at("housenumber")["postal_mask"]
                };
            };

            if (type == INavitIPC::SearchType::Country) {
                // decode country
                ret.emplace_back(SearchResult{ resultId, std::make_pair(lon,lat), decodeCountry(at) });
                nDebug() << "Country = " << ret.back().country.name;
            }
            else if (type == INavitIPC::SearchType::City) {
                ret.emplace_back(SearchResult{ resultId, std::make_pair(lon,lat), decodeCountry(at), decodeCity(at) });
                nDebug() << "Country = " << ret.back().country.name << " City = " << ret.back().city.name;
            }
            else if (type == INavitIPC::SearchType::Street) {
                ret.emplace_back(SearchResult{
                    resultId,
                    std::make_pair(lon,lat),
                    decodeCountry(at),
                    decodeCity(at),
                    decodeStreet(at) });
                nDebug() << "Country = " << ret.back().country.name << " City = " << ret.back().city.name
                         << " Street = " << ret.back().street.name;
            }
            else if (type == INavitIPC::SearchType::Address) {
                ret.emplace_back(SearchResult{
                    resultId,
                    std::make_pair(lon,lat),
                    decodeCountry(at),
                    decodeCity(at),
                    decodeStreet(at),
                    decodeHouse(at) });
                nDebug() << "Country = " << ret.back().country.name << " City = " << ret.back().city.name
                         << " Street = " << ret.back().street.name << " House = " << ret.back().house.name;
            }
        }
        catch (const DBus::Error& ex) {
            nInfo() << "Finished reading results " << ex.what();
            break;
        }
    }
    return ret;
}

void NavitDBus::selectSearchResult(INavitIPC::SearchType type, std::int32_t id)
{
    const std::string attr = convert(type);

    nInfo() << "Select search result. id= " << id << " type = " << attr;
    DBusHelpers::call("select", *(d->searchObject.get()), attr, id, 1);
}

void NavitDBus::finishSearch()
{
    DebugDBusCall db{ "finish_search" };
    nInfo() << "Destroying search list";
    if (!d->searchObject) {
        nError() << "Search wasn't startd";
        return;
    }

    DBusHelpers::call("destroy", *(d->searchObject.get()));
    nInfo() << "Search list destroyed";
    d->searchObject.reset();
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
