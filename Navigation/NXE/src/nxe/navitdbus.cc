#include "navitdbus.h"
#include "log.h"
#include "dbuscontroller.h"
#include "dbus_helpers.hpp"
#include "concurrent_queue.hpp"

#include <thread>
#include <chrono>
#include <map>
#include <bitset>
#include <dbus-c++/dbus.h>

#include <boost/signals2/signal.hpp>
#include <boost/format.hpp>
#include <boost/variant.hpp>

namespace {
const std::string navitDBusDestination = "org.navit_project.navit";
const std::string navitDBusPath = "/org/navit_project/navit/navit/0";
const std::string navitDBusInterface = "org.navit_project.navit.navit";
const std::string rootNavitDBusInterface = "org.navit_project.navit";
const std::string searchNavitDBusInterface = "org.navit_project.navit.search_list";

const std::string routeNavitDBusPath = "/org/navit_project/navit/default_navit/default_route";
const std::string routeNavitDBusInterface = "org.navit_project.navit.route";

const std::string trackingNavitDBusPath = "/org/navit_project/navit/default_navit/default_tracking";
const std::string trackingNavitDBusInterface = "org.navit_project.navit.tracking";

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

struct DBusQueuedMessage {
    enum class Type {
        Ping = 0,
        _Quit, // this Quit is reserved to quit dbus thread
        Quit, // This quit closes NavIt
        SetZoom,
        Zoom,
        Render,
        Orientation,
        SetOrientation,
        SetCenter,
        Resize,
        SetDestination,
        SetPosition,
        AddWaypoint,
        ClearDestination,
        SetScheme,
        SetPitch,
        SearchPOI,
        CurrentCenter,
        Search,
        DestroySearch,
        SelectSearch,
        SetTracking,
        Distance,
        Eta,
        CurrentStreet,
        ZoomToRoute,
        AddMapMarker,
        ClearMapMarker
    } type;
    typedef boost::variant<int,
        std::string,
        std::pair<int, int>, // for resize
        std::pair<std::string, std::string>, // for setDestination, searchPOI
        DBus::Struct<int, std::string>, // for setPosition
        std::uint16_t, // for pitch
        std::pair<NXE::INavitIPC::SearchType, std::string>, // for search
        std::pair<NXE::INavitIPC::SearchType, std::int32_t>, // for select search
        bool> VariantType;
    VariantType value;
};
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

// my eyes ;(
#define ENUM(type)                           \
    {                                        \
        DBusQueuedMessage::Type::type, #type \
    }
inline std::ostream& operator<<(std::ostream& os, DBusQueuedMessage::Type t)
{
    static std::map<DBusQueuedMessage::Type, std::string> mapped{
        ENUM(Ping),
        ENUM(_Quit),
        ENUM(Quit),
        ENUM(SetZoom),
        ENUM(Zoom),
        ENUM(Render),
        ENUM(Orientation),
        ENUM(SetOrientation),
        ENUM(SetCenter),
        ENUM(Resize),
        ENUM(SetDestination),
        ENUM(SetPosition),
        ENUM(AddWaypoint),
        ENUM(ClearDestination),
        ENUM(SetScheme),
        ENUM(SetPitch),
        ENUM(SearchPOI),
        ENUM(CurrentCenter),
        ENUM(Search),
        ENUM(SelectSearch),
        ENUM(DestroySearch),
        ENUM(SetTracking),
        ENUM(Distance),
        ENUM(Eta),
        ENUM(CurrentStreet),
        ENUM(ZoomToRoute),
        ENUM(AddMapMarker),
        ENUM(ClearMapMarker)
    };
    os << mapped.at(t);
    return os;
}

namespace NXE {

struct NavitDBusObjectProxy : public ::DBus::InterfaceProxy, public ::DBus::ObjectProxy {
    NavitDBusObjectProxy(const std::string& interface, ::DBus::Connection& con)
        : ::DBus::InterfaceProxy(interface)
        , ::DBus::ObjectProxy(con, navitDBusPath, navitDBusDestination.c_str())
    {
        dbusDebug() << "Connect to signal";
        connect_signal(NavitDBusObjectProxy, signal, signalCallback);
        connect_signal(NavitDBusObjectProxy, startup, startupCallback);
    }

    void signalCallback(const ::DBus::SignalMessage& sig)
    {
        dbusTrace() << "Signal callback";
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

        bool isRoutingSignal = std::find_if(res.begin(), res.end(), [](const std::pair<std::string, DBus::Variant>& val) -> bool {
            if (val.first == "type") {
                const std::string strVal = DBusHelpers::getFromIter<std::string>(val.second.reader());
                return strVal == "routing";
            }
            return false;

        }) != res.end();

        bool isPointClicked = std::find_if(res.begin(), res.end(), [](const std::pair<std::string, ::DBus::Variant>& val) -> bool {
            return val.first == "click_coord_geo";
        }) != res.end();

        bool isTap = std::find_if(res.begin(), res.end(), [](const std::pair<std::string, ::DBus::Variant>& val) -> bool {
            return val.first == "tap_coord_geo";
        }) != res.end();

        if (isSpeechSignal) {
            dbusTrace() << "Speech callback";
            auto dataIter = std::find_if(res.begin(), res.end(), [](const std::pair<std::string, ::DBus::Variant>& val) -> bool {
                return val.first == "data";
            });

            if (dataIter != res.end()) {
                std::string data = DBusHelpers::getFromIter<std::string>(dataIter->second.reader());
                dbusTrace() << " I have to say " << data;
                speechSignal(data);
            }
        }
        else if (isRoutingSignal) {
            dbusTrace() << "Routing signal!";
            auto dataIter = std::find_if(res.begin(), res.end(), [](const std::pair<std::string, ::DBus::Variant>& val) -> bool {
                return val.first == "data";
            });

            if (dataIter != res.end()) {
                std::string data = DBusHelpers::getFromIter<std::string>(dataIter->second.reader());
                dbusTrace() << " Routing data=" << data;
                routingSignal(data);
            }
        }
        else if (isPointClicked) {
            dbusTrace() << "Point callback";
            auto point = unpackPointClicked(res);
            pointClickedSignal(point);
        }
        else if (isTap) {
            dbusTrace() << "Tap event received";
            auto point = unpackPointClicked(res);
            tapSignal(point);
        }

        inProgress = false;
    }

    void startupCallback(const ::DBus::SignalMessage&)
    {
        inProgress = true;
        dbusDebug() << "Navit has started";
        initializedSignal();
        inProgress = false;
    }

    PointClicked unpackPointClicked(const std::vector<std::pair<std::string, ::DBus::Variant> >& dictionary)
    {
        dbusDebug() << "Unpacking point";
        double longitude, latitude;
        PointClicked::Info oneEntry;
        PointClicked::ItemArrayType items;
        std::bitset<5> haveItems{ 0 };
        std::for_each(dictionary.begin(), dictionary.end(), [&](const std::pair<std::string, ::DBus::Variant>& p) {
            dbusDebug() << "Entry= " << p.first;
            if (p.first == "click_coord_geo" || p.first == "tap_coord_geo") {
                std::vector<double> coords = DBusHelpers::getFromIter<std::vector<double>>(p.second.reader());
                longitude = coords.at(0);
                latitude = coords.at(1);
                haveItems.set(0);
            } else if(p.first == "item_type") {
                oneEntry.type = DBusHelpers::getFromIter<std::string>(p.second.reader());
                haveItems.set(1);
            } else if(p.first == "label") {
                oneEntry.label = DBusHelpers::getFromIter<std::string>(p.second.reader());
                haveItems.set(2);
            } else if(p.first == "curr_position_distance") {
                oneEntry.distance = DBusHelpers::getFromIter<std::int32_t>(p.second.reader());
                haveItems.set(3);
            } else if(p.first == "address") {
                oneEntry.address = DBusHelpers::getFromIter<std::string>(p.second.reader());
                haveItems.set(4);
            }

            if (haveItems.all()) {
                dbusDebug() << "Adding " << oneEntry.type << " " << oneEntry.label
                            << " dist=" << oneEntry.distance;
                items.push_back(oneEntry);
                oneEntry.type = "";
                oneEntry.label = "";
                oneEntry.distance = 0;
                oneEntry.address = "";
                haveItems.reset();
            }
        });

        PointClicked point{ Position{ latitude, longitude }, items };

        dbusDebug() << "Clicked item " << point;

        return point;
    }

    INavitIPC::SpeechSignalType speechSignal;
    INavitIPC::PointClickedSignalType pointClickedSignal;
    INavitIPC::PointClickedSignalType tapSignal;
    INavitIPC::InitializedSignalType initializedSignal;
    INavitIPC::RoutingSignalType routingSignal;
    bool inProgress = false;
};

struct NavitSearchObjectProxy : public ::DBus::InterfaceProxy, public ::DBus::ObjectProxy {
    NavitSearchObjectProxy(const std::string& searchPath, ::DBus::Connection& con)
        : ::DBus::InterfaceProxy(searchNavitDBusInterface)
        , ::DBus::ObjectProxy(con, searchPath, navitDBusDestination.c_str())
    {
    }
};

struct NavitRouteObjectProxy : public DBus::InterfaceProxy, public DBus::ObjectProxy {
    NavitRouteObjectProxy(DBus::Connection& con)
        : DBus::InterfaceProxy(routeNavitDBusInterface)
        , DBus::ObjectProxy(con, routeNavitDBusPath, navitDBusDestination.c_str())
    {
    }
};
struct NavitTrackingObjectProxy : public DBus::InterfaceProxy, public DBus::ObjectProxy {
    NavitTrackingObjectProxy(DBus::Connection& con)
        : DBus::InterfaceProxy(trackingNavitDBusInterface)
        , DBus::ObjectProxy(con, trackingNavitDBusPath, navitDBusDestination.c_str())
    {
    }
};

struct NavitDBusPrivate {
    NavitDBusPrivate(::DBus::Connection& _con)
        : con(_con)
    {
        dbusMainThread = std::thread{ [this]() {
            dbusInfo() << "Staring dbus thread";
            dbusThreadRunning = true;
            DBusQueuedMessage msg;
            bool quitMessageReceived = false;
            while(!quitMessageReceived) {
                try {
                    spsc_queue.wait_and_pop(msg);
                    dbusTrace() << "DBus SPSC received " << msg.type;
                    // we have something
                    switch (msg.type) {
                    case DBusQueuedMessage::Type::Ping:
                    {
                        dbusTrace() << "Ping";
                        break;
                    }
                    case DBusQueuedMessage::Type::_Quit:
                        dbusTrace() << "Quiting dbus processing thread";
                        quitMessageReceived = true;
                        break;
                    case DBusQueuedMessage::Type::Quit:
                        dbusTrace() << "Quit Navit";
                        DBusHelpers::call("quit", *(object.get()));
                        break;
                    case DBusQueuedMessage::Type::SetZoom:
                    {
                        int newZoomValue = boost::get<int>(msg.value);
                        dbusDebug() << "Setting zoom to=" << newZoomValue;
                        DBusHelpers::setAttr("zoom", *(object.get()), newZoomValue);
                        dbusTrace() << "Setting zoom finished";
                        break;
                    }
                    case DBusQueuedMessage::Type::Zoom:
                    {
                        dbusDebug() << "Getting zoom";
                        int zoom = DBusHelpers::getAttr<int>("zoom", *(object.get()));
                        zoomSignal(zoom);
                        break;
                    }
                    case DBusQueuedMessage::Type::Render:
                        DBusHelpers::callNoReply("draw", *(object.get()));
                        break;
                    case DBusQueuedMessage::Type::Orientation:
                        orientationSignal(DBusHelpers::getAttr<int>("orientation", *(object.get())));
                        break;
                    case DBusQueuedMessage::Type::SetOrientation:
                        DBusHelpers::setAttr("orientation", *(object.get()), boost::get<int>(msg.value));
                        break;
                    case DBusQueuedMessage::Type::SetCenter:
                        dbusTrace() << "Set center, center= " << boost::get<std::string>(msg.value);
                        DBusHelpers::call("set_center_by_string", *(object.get()), boost::get<std::string>(msg.value));
                        break;
                    case DBusQueuedMessage::Type::Resize:
                    {
                        auto params = boost::get<std::pair<int,int>>(msg.value);
                        DBusHelpers::callNoReply("resize", *(object.get()), params.first, params.second);
                        break;
                    }
                    case DBusQueuedMessage::Type::SetDestination:
                    {
                        auto params = boost::get<std::pair<std::string, std::string>>(msg.value);
                        DBusHelpers::call("set_destination", *(object.get()), params.first, params.second);
                        navigation = true;
                        navigationChangedSignal(navigation);
                        break;
                    }
                    case DBusQueuedMessage::Type::SetPosition:
                    {
                        auto params = boost::get<DBus::Struct<int, std::string>>(msg.value);
                        DBusHelpers::callNoReply("set_center", *(object.get()), params);
                        break;
                    }
                    case DBusQueuedMessage::Type::AddWaypoint:
                        DBusHelpers::call("add_waypoint", *(object.get()), boost::get<std::string>(msg.value));
                        break;
                    case DBusQueuedMessage::Type::ClearDestination:
                        dbusDebug() << "Clear destination";
                        DBusHelpers::call("clear_destination", *(object.get()));
                        navigation = false;
                        navigationChangedSignal(navigation);
                        break;
                    case DBusQueuedMessage::Type::SetScheme:
                        DBusHelpers::callNoReply("set_layout", *(object.get()), boost::get<std::string>(msg.value));
                        break;
                    case DBusQueuedMessage::Type::SetPitch:
                        DBusHelpers::setAttr("pitch", *(object.get()), static_cast<std::int32_t>(boost::get<std::uint16_t>(msg.value)));
                        break;
                    case DBusQueuedMessage::Type::SearchPOI:
                    {
                        auto params = boost::get<std::pair<std::string, std::string>>(msg.value);
                        DBusHelpers::call("search_pois", *(object.get()), params.first, params.second);
                        searchPoiSignal();
                        break;
                    }
                    case DBusQueuedMessage::Type::CurrentCenter:
                    {
                        auto ret = DBusHelpers::getAttr<DBus::Struct<double, double> >("center", *(object.get()));
                        dbusDebug() << "Current center lon= " << ret._2 <<" lat= "<< ret._1;
                        currentCenterSignal(NXE::Position{ret._2, ret._1});
                        break;
                    }
                    case DBusQueuedMessage::Type::Search:
                    {
                        auto params = boost::get<std::pair<INavitIPC::SearchType, std::string>>(msg.value);
                        searchSignal(search(params.first, params.second), params.first);
                        break;
                    }
                    case DBusQueuedMessage::Type::SelectSearch:
                    {
                        auto params = boost::get<std::pair<INavitIPC::SearchType, std::int32_t>>(msg.value);
                        const std::string attr = convert(params.first);
                        dbusTrace() << "Selecting search " << params.second;
                        DBusHelpers::call("select", *(searchObject.get()), attr, params.second, 1);
                        break;

                    }
                    case DBusQueuedMessage::Type::DestroySearch:
                    {
                        DBusHelpers::call("destroy", *(searchObject.get()));
                        searchObject.reset();
                        break;
                    }
                    case DBusQueuedMessage::Type::SetTracking:
                    {
                        dbusDebug() << "Setting tracking to " << boost::get<bool>(msg.value);
                        DBusHelpers::setAttr("follow_cursor", *(object.get()), boost::get<bool>(msg.value));
                        break;
                    }
                    case DBusQueuedMessage::Type::Distance:
                    {
                        if (!navigationCancelled) {
                            std::int32_t distance = DBusHelpers::getAttr<int>("destination_length", *(routeObject.get()));
                            distanceSignal(distance);
                        }
                        break;
                    }
                    case DBusQueuedMessage::Type::Eta:
                    {
                        if(!navigationCancelled) {
                            std::int32_t eta = DBusHelpers::getAttr<std::int32_t>("destination_time", *(routeObject.get()));
                            etaSignal(eta);
                        }
                        break;
                    }
                    case DBusQueuedMessage::Type::CurrentStreet:
                    {
                        DBus::Message msg = DBusHelpers::call("get_attr", *(trackingObject.get()), std::string{"street_name"});
                        auto iter = msg.reader();
                        std::string ss;
                        DBus::Variant v;
                        iter >> ss >> v;
                        auto streetName = DBusHelpers::getFromIter<std::string> (v.reader());
                        currentStreetSignal(streetName);
                        break;
                    }
                    case DBusQueuedMessage::Type::ZoomToRoute:
                    {
                        DBusHelpers::call("zoom_to_route", *(object.get()));
                        break;
                    }
                    case DBusQueuedMessage::Type::AddMapMarker:
                    {
                        const std::string geo = boost::get<std::string>(msg.value);
                        DBusHelpers::call("draw_sel_point", *(object.get()), geo);
                        break;
                    }
                    case DBusQueuedMessage::Type::ClearMapMarker:
                    {
                        DBusHelpers::call("clear_sel_point", *(object.get()));
                        break;
                    }

                    } // switch end
                } catch(const std::exception& ex) {
                    dbusError() << "An exception occured during dbus call " << msg.type << " message = " << ex.what();
                }
            }
            dbusInfo() << "Processing thread is done and it will be no more!";
            dbusThreadRunning = false;
        } };
    }

    void createSearchList()
    {
        // create a new search
        ::DBus::Path path{ "/org/navit_project/navit/default_navit/default_mapset" };
        ::DBus::Message retValue = DBusHelpers::call("search_list_new", *(rootObject.get()), path);
        ::DBus::MessageIter iter = retValue.reader();

        ::DBus::Path searchListPath;
        iter >> searchListPath;

        dbusDebug() << "Search object is " << searchListPath;

        searchObject.reset(new NavitSearchObjectProxy{ searchListPath, con });
    }

    SearchResults search(INavitIPC::SearchType type, const std::string& searchString)
    {
        if (!searchObject)
            throw std::runtime_error("startSearch not called");

        dbusDebug() << "Searching for " << searchString;
        std::string tag = convert(type);

        dbusTrace() << "Tag =" << tag;
        SearchResults ret;

        DBus::Variant var;
        DBus::MessageIter variantIter = var.writer();
        variantIter << searchString;

        DBusHelpers::call("search", *(searchObject.get()), tag, var, static_cast<int>(1));
        while (true) {
            try {
                DBus::Message results = DBusHelpers::call("get_result", *(searchObject.get()));
                dbusDebug() << "Received result";
                DBus::MessageIter resultsIter{ results.reader() };

                std::int32_t resultId;
                DBus::Struct<double, double> position;
                std::string currPosDistance;
                DBus::Variant distanceVar;
                typedef std::map<std::string, std::map<std::string, ::DBus::Variant> > LocationDBusType;
                LocationDBusType at;
                double lat, lon;

                resultsIter >> resultId >> position;
                resultsIter >> currPosDistance >> distanceVar;
                resultsIter >> at;
                lat = position._2;
                lon = position._1;

                dbusDebug() << "Search results: " << resultId << " Pos = "
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
                        postalMask = DBusHelpers::getFromIter<std::string>(isPostalMask->second.reader());
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
                    std::string postal, postalMask;
                    auto isPostal = map.at("housenumber").find("postal");
                    if (isPostal != map.at("housenumber").end()) {
                        postal = DBusHelpers::getFromIter<std::string>(isPostal->second.reader());
                    }
                    auto isPostalMask = map.at("housenumber").find("postal_mask");
                    if (isPostalMask != map.at("housenumber").end()) {
                        postalMask = DBusHelpers::getFromIter<std::string>(isPostalMask->second.reader());
                    }
                    return SearchResult::HouseNumber{
                            map.at("housenumber").at("name"),
                                postal,
                                postalMask
                    };
                };

                if (type == INavitIPC::SearchType::Country) {
                    // decode country
                    ret.emplace_back(SearchResult{ resultId, std::make_pair(lon, lat), decodeCountry(at) });
                    dbusTrace() << "Country = " << ret.back().country.name;
                }
                else if (type == INavitIPC::SearchType::City) {
                    ret.emplace_back(SearchResult{ resultId, std::make_pair(lon, lat), decodeCountry(at), decodeCity(at) });
                    dbusTrace() << "Country = " << ret.back().country.name << " City = " << ret.back().city.name;
                }
                else if (type == INavitIPC::SearchType::Street) {
                    ret.emplace_back(SearchResult{
                        resultId,
                        std::make_pair(lon, lat),
                        decodeCountry(at),
                        decodeCity(at),
                        decodeStreet(at) });
                    dbusTrace() << "Country = " << ret.back().country.name << " City = " << ret.back().city.name
                                << " Street = " << ret.back().street.name;
                }
                else if (type == INavitIPC::SearchType::Address) {
                    ret.emplace_back(SearchResult{
                        resultId,
                        std::make_pair(lon, lat),
                        decodeCountry(at),
                        decodeCity(at),
                        decodeStreet(at),
                        decodeHouse(at) });
                    dbusTrace() << "Country = " << ret.back().country.name << " City = " << ret.back().city.name
                                << " Street = " << ret.back().street.name << " House = " << ret.back().house.name;
                }
            }
            catch (const DBus::Error& ex) {
                dbusInfo() << "Finished reading results " << ex.what();
                break;
            }
        }
        return ret;
    }

    std::shared_ptr<NavitDBusObjectProxy> object;
    std::shared_ptr<NavitDBusObjectProxy> rootObject;
    std::shared_ptr<NavitSearchObjectProxy> searchObject;
    std::shared_ptr<NavitRouteObjectProxy> routeObject;
    std::shared_ptr<NavitTrackingObjectProxy> trackingObject;
    DBus::Connection& con;

    std::thread dbusMainThread;
    bool dbusThreadRunning{ false };
    bool navigation{ false };
    bool navigationCancelled{ false };
    concurrent_queue<DBusQueuedMessage> spsc_queue;

    INavitIPC::IntSignalType zoomSignal;
    INavitIPC::IntSignalType orientationSignal;
    INavitIPC::EmptySignalType searchPoiSignal;
    INavitIPC::CurrentCenterSignalType currentCenterSignal;
    INavitIPC::SearchResultsSignalType searchSignal;
    INavitIPC::IntSignalType distanceSignal;
    INavitIPC::IntSignalType etaSignal;
    INavitIPC::BoolSignalType navigationChangedSignal;
    INavitIPC::StringSignalType currentStreetSignal;
};

NavitDBus::NavitDBus(DBusController& ctrl)
    : d(new NavitDBusPrivate{ ctrl.connection() })
{
    dbusDebug() << "NavitDBus::NavitDBus()";
    d->object.reset(new NavitDBusObjectProxy(navitDBusInterface, ctrl.connection()));
    d->rootObject.reset(new NavitDBusObjectProxy(rootNavitDBusInterface, ctrl.connection()));
    d->routeObject.reset(new NavitRouteObjectProxy(ctrl.connection()));
    d->trackingObject.reset(new NavitTrackingObjectProxy(ctrl.connection()));
}

NavitDBus::~NavitDBus()
{
    dbusDebug() << "Destroying navit dbus";
    quit();
}

void NavitDBus::quit()
{
    dbusInfo() << "Request Quiting Navit";
    if (!d->dbusThreadRunning) {
        dbusInfo() << "Navit probably already closed";
        return;
    }
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::Quit });
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::_Quit });
    d->dbusMainThread.join();
    dbusInfo() << "Navit DBus finished";
}

void NavitDBus::setZoom(int newZoom)
{
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::SetZoom, DBusQueuedMessage::VariantType{ newZoom } });
}

void NavitDBus::zoom()
{
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::Zoom });
}

void NavitDBus::render()
{
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::Render });
}

void NavitDBus::resize(int x, int y)
{
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::Render, DBusQueuedMessage::VariantType{ std::make_pair(x, y) } });
}

void NavitDBus::orientation()
{
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::Orientation });
}

void NavitDBus::setOrientation(int newOrientation)
{
    if (newOrientation != 0 && newOrientation != -1) {
        dbusError() << "Unable to change orientation to " << newOrientation;
        throw std::runtime_error("Unable to change orientation. Incorrect value, value can only be -1/0");
    }
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::SetOrientation, DBusQueuedMessage::VariantType{ newOrientation } });
}

void NavitDBus::setCenter(double longitude, double latitude)
{
    auto format = boost::format("geo: %1% %2%") % longitude % latitude;
    const std::string message = format.str();

    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::SetCenter, DBusQueuedMessage::VariantType{ message } });
}

void NavitDBus::setDestination(double longitude, double latitude, const std::string& description)
{
    d->navigationCancelled = false;
    auto format = boost::format("geo: %1% %2%") % longitude % latitude;
    const std::string message = format.str();
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::SetDestination, DBusQueuedMessage::VariantType{ std::make_pair(message, description) } });
}

bool NavitDBus::isNavigationRunning()
{
    return d->navigation;
}

void NavitDBus::setPosition(double longitude, double latitude)
{
    auto format = boost::format("geo: %1% %2%") % longitude % latitude;
    const std::string message = format.str();
    DBus::Struct<int, std::string> s;
    s._1 = 1;
    s._2 = message;

    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::SetPosition, DBusQueuedMessage::VariantType{ s } });
}

void NavitDBus::addWaypoint(double longitude, double latitude)
{
    auto format = boost::format("geo: %1% %2%") % longitude % latitude;
    const std::string message = format.str();
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::AddWaypoint, DBusQueuedMessage::VariantType{ message } });
}

void NavitDBus::clearDestination()
{
    d->navigationCancelled = true;
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::ClearDestination });
}

void NavitDBus::setScheme(const std::string& scheme)
{
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::SetScheme, DBusQueuedMessage::VariantType{ scheme } });
}

void NavitDBus::setPitch(std::uint16_t newPitchValue)
{
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::SetPitch, DBusQueuedMessage::VariantType{ newPitchValue } });
}

void NavitDBus::searchPOIs(double longitude, double latitude, int dist)
{
    auto format = boost::format("geo: %1% %2%") % longitude % latitude;
    auto format1 = boost::format("%1%") % dist;
    const std::string center_coord = format.str();
    const std::string distance = format1.str();

    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::SearchPOI, DBusQueuedMessage::VariantType{ std::make_pair(center_coord, distance) } });
}

void NavitDBus::currentCenter()
{
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::CurrentCenter });
}

void NavitDBus::currentStreet()
{
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::CurrentStreet });
}

void NavitDBus::startSearch()
{
    d->createSearchList();
}

void NavitDBus::search(INavitIPC::SearchType type, const std::string& searchString)
{
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::Search, DBusQueuedMessage::VariantType{ std::make_pair(type, searchString) } });
}

void NavitDBus::selectSearchResult(INavitIPC::SearchType type, std::int32_t id)
{
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::SelectSearch, DBusQueuedMessage::VariantType{ std::make_pair(type, id) } });
}

void NavitDBus::finishSearch()
{
    if (!d->searchObject) {
        dbusError() << "Search wasn't startd";
        return;
    }

    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::DestroySearch });
}

void NavitDBus::setTracking(bool tracking)
{
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::SetTracking, tracking });
}

void NavitDBus::zoomToRoute()
{
    dbusDebug() << "Zooming to route";
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::ZoomToRoute });
}

void NavitDBus::addMapMarker(double longitude, double latitude)
{
    std::string message = std::string{"geo: "} + std::to_string(longitude) + std::string{" "} + std::to_string(latitude);
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::AddMapMarker, message });
}

void NavitDBus::clearMapMarker()
{
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::ClearMapMarker });
}
void NavitDBus::distance()
{
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::Distance });
}

void NavitDBus::eta()
{
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::Eta });
}

INavitIPC::IntSignalType& NavitDBus::orientationResponse()
{
    return d->orientationSignal;
}

INavitIPC::IntSignalType& NavitDBus::zoomResponse()
{
    return d->zoomSignal;
}

INavitIPC::EmptySignalType& NavitDBus::searchPoiResponse()
{
    return d->searchPoiSignal;
}

INavitIPC::CurrentCenterSignalType& NavitDBus::currentCenterResponse()
{
    return d->currentCenterSignal;
}

INavitIPC::SearchResultsSignalType& NavitDBus::searchResponse()
{
    return d->searchSignal;
}

INavitIPC::IntSignalType& NavitDBus::distanceResponse()
{
    return d->distanceSignal;
}

INavitIPC::IntSignalType& NavitDBus::etaResponse()
{
    return d->etaSignal;
}

INavitIPC::BoolSignalType& NavitDBus::navigationChanged()
{
    return d->navigationChangedSignal;
}

INavitIPC::StringSignalType& NavitDBus::currentStreetResponse()
{
    return d->currentStreetSignal;
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

INavitIPC::PointClickedSignalType& NavitDBus::tapSignal()
{
    return d->object->tapSignal;
}

INavitIPC::InitializedSignalType& NavitDBus::initializedSignal()
{
    return d->object->initializedSignal;
}

INavitIPC::RoutingSignalType& NavitDBus::routingSignal()
{
    return d->object->routingSignal;
}

} // namespace NXE
