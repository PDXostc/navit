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
#include <boost/variant.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/atomic.hpp>

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

struct DBusQueuedMessage {
    enum class Type {
        Ping = 0,
        _Quit, // this Quit is reserved to quit dbus thread
        Quit,  // This quit closes NavIt
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
        CurrentCenter
    } type;
    typedef boost::variant<int,
        std::string,
        std::pair<int, int>, // for resize
        std::pair<std::string, std::string>, // for setDestination, searchPOI
        DBus::Struct<int, std::string>, // for setPosition
        std::uint16_t // for pitch
        > VariantType;
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
        nTrace() << "Signal callback";
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
        else if (isRoutingSignal) {
            nTrace() << "Routing signal!";
            auto dataIter = std::find_if(res.begin(), res.end(), [](const std::pair<std::string, ::DBus::Variant>& val) -> bool {
                return val.first == "data";
            });

            if (dataIter != res.end()) {
                std::string data = DBusHelpers::getFromIter<std::string>(dataIter->second.reader());
                nDebug() << " Routing data=" << data;
                routingSignal(data);
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

struct NavitDBusPrivate {
    NavitDBusPrivate(::DBus::Connection& _con)
        : con(_con)
    {
        dbusMainThread = std::thread{ [this]() {
            dbusInfo() << "Staring dbus thread";
            DBusQueuedMessage msg;
            bool quitMessageReceived = false;
            while(!quitMessageReceived) {
                if(spsc_queue.pop(msg)) {
                    // we have something
                    switch (msg.type) {
                    case DBusQueuedMessage::Type::_Quit:
                        dbusInfo() << "Quiting dbus processing thread";
                        quitMessageReceived = true;
                        break;
                    case DBusQueuedMessage::Type::Quit:
                        DBusHelpers::callNoReply("quit", *(object.get()));
                        break;
                    case DBusQueuedMessage::Type::SetZoom:
                    {
                        int newZoomValue = boost::get<int>(msg.value);
                        dbusInfo() << "Setting zoom to=" << newZoomValue;
                        DBusHelpers::setAttr("zoom", *(object.get()), newZoomValue);
                        dbusInfo() << "Setting zoom finished";
                        break;
                    }
                    case DBusQueuedMessage::Type::Zoom:
                    {
                        nDebug() << "Getting zoom";
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
                        DBusHelpers::callNoReply("set_center_by_string", *(object.get()), boost::get<std::string>(msg.value));
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
                        DBusHelpers::callNoReply("set_destination", *(object.get()), params.first, params.second);
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
                        DBusHelpers::callNoReply("clear_destination", *(object.get()));
                        break;
                    case DBusQueuedMessage::Type::SetScheme:
                        DBusHelpers::callNoReply("set_layout", *(object.get()), boost::get<std::string>(msg.value));
                        break;
                    case DBusQueuedMessage::Type::SetPitch:
                        DBusHelpers::setAttr("pitch", *(object.get()), boost::get<std::uint16_t>(msg.value));
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
                        nInfo() << "Current center";
                        auto ret = DBusHelpers::getAttr<DBus::Struct<double, double> >("center", *(object.get()));
                        nInfo() << "Current center " << ret._1 << ret._2;
                        currentCenterSignal(NXE::Position{ret._1, ret._2});
                        break;
                    }
                    } // switch end
                }
            }
            dbusInfo() << "Processing thread is done and it will be no more!";
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

        nDebug() << "Search object is " << searchListPath;

        searchObject.reset(new NavitSearchObjectProxy{ searchListPath, con });
    }

    std::shared_ptr<NavitDBusObjectProxy> object;
    std::shared_ptr<NavitDBusObjectProxy> rootObject;
    std::shared_ptr<NavitSearchObjectProxy> searchObject;
    DBus::Connection& con;

    std::thread dbusMainThread;
    boost::lockfree::spsc_queue<DBusQueuedMessage, boost::lockfree::capacity<1024> > spsc_queue;

    INavitIPC::IntSignalType zoomSignal;
    INavitIPC::IntSignalType orientationSignal;
    INavitIPC::EmptySignalType searchPoiSignal;
    INavitIPC::CurrentCenterSignalType currentCenterSignal;
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
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::_Quit });
    d->dbusMainThread.join();
}

void NavitDBus::quit()
{
    dbusInfo() << "Quiting Navit";
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::Quit });
}

void NavitDBus::setZoom(int newZoom)
{
    dbusInfo() << "Setting zoom = " << newZoom;
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
    dbusInfo() << "Resizing [" << x << "x" << y << "]";
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::Render, DBusQueuedMessage::VariantType{ std::make_pair(x, y) } });
}

void NavitDBus::orientation()
{
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::Orientation });
    //    return d->orientationSignal;
}

void NavitDBus::setOrientation(int newOrientation)
{
    nDebug() << "Changing orientation to " << newOrientation;
    if (newOrientation != 0 && newOrientation != -1) {
        nError() << "Unable to change orientation to " << newOrientation;
        throw std::runtime_error("Unable to change orientation. Incorrect value, value can only be -1/0");
    }
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::SetOrientation, DBusQueuedMessage::VariantType{ newOrientation } });
}

void NavitDBus::setCenter(double longitude, double latitude)
{
    dbusInfo() << "Setting center lon= " << longitude << " lat= " << latitude;
    auto format = boost::format("geo: %1% %2%") % longitude % latitude;
    const std::string message = format.str();

    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::SetCenter, DBusQueuedMessage::VariantType{ message } });
}

void NavitDBus::setDestination(double longitude, double latitude, const std::string& description)
{
    nDebug() << "Setting destionation to. name= " << description;
    auto format = boost::format("geo: %1% %2%") % longitude % latitude;
    const std::string message = format.str();
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::SetDestination, DBusQueuedMessage::VariantType{ std::make_pair(message, description) } });
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
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::ClearDestination });
}

void NavitDBus::setScheme(const std::string& scheme)
{
    nDebug() << "Setting scheme to " << scheme;
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::SetScheme, DBusQueuedMessage::VariantType{ scheme } });
}

void NavitDBus::setPitch(std::uint16_t newPitchValue)
{
    std::int32_t pitchVal = static_cast<std::int32_t>(newPitchValue);
    nDebug() << "Setting pitch to = " << pitchVal;
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::SetPitch, DBusQueuedMessage::VariantType{ newPitchValue } });
}

void NavitDBus::searchPOIs(double longitude, double latitude, int dist)
{
    nDebug() << "searchPOIs in " << dist << " distance";
    auto format = boost::format("geo: %1% %2%") % longitude % latitude;
    auto format1 = boost::format("%1%") % dist;
    const std::string center_coord = format.str();
    const std::string distance = format1.str();

    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::SearchPOI, DBusQueuedMessage::VariantType{ std::make_pair(center_coord, distance) } });
}

void NavitDBus::currentCenter()
{
    dbusInfo() << "Requesting current center";
    d->spsc_queue.push(DBusQueuedMessage{ DBusQueuedMessage::Type::CurrentCenter });
}

void NavitDBus::startSearch()
{
    nInfo() << "Creating new search";
    d->createSearchList();
}

SearchResults NavitDBus::search(INavitIPC::SearchType type, const std::string& searchString)
{
    if (!d->searchObject)
        throw std::runtime_error("startSearch not called");

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
            DBus::Struct<double, double> position;
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
                ret.emplace_back(SearchResult{ resultId, std::make_pair(lon, lat), decodeCountry(at) });
                nDebug() << "Country = " << ret.back().country.name;
            }
            else if (type == INavitIPC::SearchType::City) {
                ret.emplace_back(SearchResult{ resultId, std::make_pair(lon, lat), decodeCountry(at), decodeCity(at) });
                nDebug() << "Country = " << ret.back().country.name << " City = " << ret.back().city.name;
            }
            else if (type == INavitIPC::SearchType::Street) {
                ret.emplace_back(SearchResult{
                    resultId,
                    std::make_pair(lon, lat),
                    decodeCountry(at),
                    decodeCity(at),
                    decodeStreet(at) });
                nDebug() << "Country = " << ret.back().country.name << " City = " << ret.back().city.name
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
    nInfo() << "Destroying search list";
    if (!d->searchObject) {
        nError() << "Search wasn't startd";
        return;
    }

    DBusHelpers::call("destroy", *(d->searchObject.get()));
    nInfo() << "Search list destroyed";
    d->searchObject.reset();
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

INavitIPC::RoutingSignalType& NavitDBus::routingSignal()
{
    return d->object->routingSignal;
}

} // namespace NXE
