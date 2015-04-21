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
        : ::DBus::InterfaceProxy("org.nxe.MapDownloader")
        , ::DBus::ObjectProxy(con, "/org/nxe/MapDownloader", "org.nxe.MapDownloader")
    {
        connect_signal(Proxy, progress, progressCallback);
        connect_signal(Proxy, finished, finishedCallback);
        connect_signal(Proxy, error, errorCallback);
    }

    void progressCallback (const ::DBus::SignalMessage &sig)
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

    void finishedCallback (const ::DBus::SignalMessage &sig)
    {
        std::string mapName;
        auto reader = sig.reader();
        reader >> mapName;
        nTrace() << "Finished " << mapName;
        if (listener.finishedCb) {
            listener.finishedCb(mapName);
        }
    }

    void errorCallback( const ::DBus::SignalMessage &sig)
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

std::vector<std::string> MapDownloaderDBus ::availableMaps() 
{
    nInfo() << "Calling available maps IPC";
    std::vector<std::string> ret;
    auto retMessage = DBus::call("availableMaps", *(d->proxy.get()));
    auto iter = retMessage.reader();
    iter >> ret;
    return ret;
}

bool MapDownloaderDBus::download(const std::string &region)
{
    nInfo() << "Downloading region= " << region;
    bool bRet {false};

    auto reply = DBus::call("download", *(d->proxy.get()), region);
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
    DBus::call("cancel", *(d->proxy.get()), region);
}

bool MapDownloaderDBus::setOutputDirectory(const std::string &outDirectory)
{
    auto message = DBus::call("setOutputDirectory", *(d->proxy.get()), outDirectory);
    auto messageIter = message.reader();

    bool val;
    messageIter >> val;
    return val;
}

}
