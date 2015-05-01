#include "mapdownloaderdbusserver.h"
#include "mapdownloader.h"
#include "mdlog.h"

namespace md {

struct MapDownloaderDBusServerPrivate {
    //          mapUrl     mapName
    std::map<std::string, std::string> requests;
    MapDownloader downloader;
};

MapDownloaderDBusServer::MapDownloaderDBusServer(DBus::Connection& connection)
    : DBus::ObjectAdaptor(connection, "/org/nxe/mapdownloader")
    , d(new MapDownloaderDBusServerPrivate)
{
    d->downloader.setCbError([this](const std::string& url, const std::string& errorStr) {
        mdError() << "Error " << errorStr << " while downloading " << url;
        error(d->requests[url], errorStr);
        auto it = d->requests.find(url);
        if (it != d->requests.end() ) {
            mdInfo() << "Request " << it->second << " had error " << errorStr;
            d->requests.erase(it);
        }
    });

    d->downloader.setCbProgress([this](const std::string& url, long p, long t) {
        mdInfo() << "Progress [ " << p/1024 << " kB /" << t/1024 << " kB ] for url= " << url;
        progress(d->requests[url], p, t);
    });

    d->downloader.setCbOnFinished([this](const std::string& url) {
        auto it = d->requests.find(url);
        finished(d->requests[url]);
        if (it != d->requests.end() ) {
            mdInfo() << "Request " << it->second << " finished";
            d->requests.erase(it);
        }
    });
}

MapDownloaderDBusServer::~MapDownloaderDBusServer()
{
    mdInfo() << __PRETTY_FUNCTION__;
    std::for_each(d->requests.begin(), d->requests.end(),[this](const std::pair<std::string, std::string> &p) {
        d->downloader.cancel(p.first);
    });
}

bool MapDownloaderDBusServer::setOutputDirectory(const std::string& path)
{
    mdInfo() << "Setting new output path " << path;
    return d->downloader.setMapFileDir(path);
}

bool MapDownloaderDBusServer::download(const std::string& mapName)
{
    auto it = std::find_if(d->requests.begin(), d->requests.end(),
                           [&mapName](const std::pair<std::string, std::string>& pair) -> bool {
        return mapName == pair.second;
    });

    if (it == d->requests.end() ) {
        const std::string url = d->downloader.download(mapName);
        if (!url.empty()) {
            d->requests[url] = mapName;
            mdInfo() << "Downloading " << mapName << " url is= " << url;
        }
        return !url.empty();
    } else {
        mdError() << "Already downloading " << mapName;
        return false;
    }
}

void MapDownloaderDBusServer::reportProgress(const bool& enable)
{
    d->downloader.enableReportProgress(enable);
}

void MapDownloaderDBusServer::cancel(const std::string &mapName)
{
    mdDebug() << "Trying to cancel " << mapName;
    mdTrace() << "size before " << d->requests.size();
    auto it = std::find_if(d->requests.begin(), d->requests.end(),
                           [&mapName](const std::pair<std::string, std::string>& pair) -> bool {
        return mapName == pair.second;
    });

    if (it != d->requests.end()) {
        d->downloader.cancel(it->first);
        d->requests.erase(it);
        mdDebug() << "Requests canceled and erased";
        mdTrace() << "size after " << d->requests.size();
    } else {
        mdError() << "Unable to find " << mapName;
    }
}

std::vector<::DBus::Struct<std::string, uint64_t, bool> > MapDownloaderDBusServer::maps()
{
    mdInfo() << "Requesting maps";
    auto maps = d->downloader.maps();
    mdInfo() << "maps size = " << maps.size();
    std::vector<::DBus::Struct<std::string, std::uint64_t, bool>> ret;
    std::for_each(maps.begin(), maps.end(), [&ret](const MapEntry& me) {
        ::DBus::Struct<std::string, std::uint64_t, bool> oneVal;
        oneVal._1 = me.name;
        oneVal._2 = me.size;
        oneVal._3 = me.isDownloaded;
        ret.emplace_back(oneVal);
    });

    return ret;
}

} // namespace md
