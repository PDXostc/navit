#include "mapdownloaderdbus.h"
#include "dbuscontroller.h"
#include "dbus_helpers.hpp"
#include "log.h"

#include <thread>
#include <dbus-c++/dbus.h>
#include <cstdint>

namespace NXE {

struct Proxy : public ::DBus::InterfaceProxy, public ::DBus::ObjectProxy 
{
    Proxy(::DBus::Connection con)
        : ::DBus::InterfaceProxy("org.nxe.mapdownloader")
        , ::DBus::ObjectProxy(con, "/org/nxe/mapdownloader", "org.nxe.mapdownloader")
    {
        connect_signal(Proxy, progress, progressCallback);
        connect_signal(Proxy, finished, finishedCallback);
        connect_signal(Proxy, error, errorCallback);
    }

    void progressCallback (const DBus::SignalMessage &sig)
    {
        nTrace() << "Progress callback";
        auto iter = sig.reader();
        std::uint64_t now, total;
        std::string mapName;
        iter >> mapName >> now >> total;
        if (listener.progressCb) {
            listener.progressCb(mapName, now, total);
        }
    }

    void finishedCallback (const DBus::SignalMessage &sig)
    {
        std::string mapName;
        auto reader = sig.reader();
        reader >> mapName;
        nTrace() << "Finished " << mapName;
        if (listener.finishedCb) {
            listener.finishedCb(mapName);
        }
    }

    void errorCallback( const DBus::SignalMessage &sig)
    {
        std::string map, error;

        auto iter = sig.reader();
        iter >> map >> error;
        nError() << "An error occured during map downloading. Map= " << map << " error= " << error;
        if (listener.errorCb) {
            listener.errorCb(map, error);
        }
    }

    MapDownloaderListener listener;
};

struct MapDownloaderDBusPrivate {
    std::unique_ptr<Proxy> proxy;
};

MapDownloaderDBus::MapDownloaderDBus(DBusController& ctrl):
    d(new MapDownloaderDBusPrivate)
{
    d->proxy.reset(new Proxy {ctrl.connection()});
}

MapDownloaderDBus::~MapDownloaderDBus()
{
}

std::vector<MapInfo> MapDownloaderDBus::maps()
{
    nInfo() << "Calling available maps IPC";
    std::vector<DBus::Struct<std::string, std::uint64_t, bool>> result;
    std::vector<MapInfo> maps;
    auto retMessage = DBusHelpers::call("maps", *(d->proxy.get()));
    auto iter = retMessage.reader();
    iter >> result;
    std::for_each(result.begin(), result.end(), [&maps] (const ::DBus::Struct<std::string, std::uint64_t, bool>& entry){
        maps.emplace_back(MapInfo{entry._1, entry._2, entry._3});

    });
    return maps;
}

bool MapDownloaderDBus::download(const std::string &region)
{
    nInfo() << "Downloading region= " << region;
    bool bRet {false};

    auto reply = DBusHelpers::call("download", *(d->proxy.get()), region);
    auto replyIter = reply.reader();
    replyIter >> bRet;
    return bRet;
}

void MapDownloaderDBus::setListener(const MapDownloaderListener &listener)
{
    if (d->proxy) {
        d->proxy->listener = listener;
    }
}

void MapDownloaderDBus::cancel(const std::string &region)
{
    DBusHelpers::call("cancel", *(d->proxy.get()), region);
}

bool MapDownloaderDBus::setOutputDirectory(const std::string &outDirectory)
{
    auto message = DBusHelpers::call("setOutputDirectory", *(d->proxy.get()), outDirectory);
    auto messageIter = message.reader();

    bool val;
    messageIter >> val;
    return val;
}

void MapDownloaderDBus::removeAllMaps()
{
    DBusHelpers::callNoReply("removeAllMaps", *(d->proxy.get()));
}

}
