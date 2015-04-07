#include "mapdownloaderdbusserver.h"
#include "mapdownloader.h"
#include "mdlog.h"

namespace md {

struct MapDownloaderDBusServerPrivate {
    //          mapName     mapUrl
    std::map<std::string, std::string> requests;
    MapDownloader downloader;
};

MapDownloaderDBusServer::MapDownloaderDBusServer(DBus::Connection& connection)
    : DBus::ObjectAdaptor(connection, "/org/nxe/MapDownloader")
    , d(new MapDownloaderDBusServerPrivate)
{
    d->downloader.setCbError([this](const std::string& url, const std::string& error) {
        mdError() << "Error " << error << " while downloading " << url;
    });

    d->downloader.setCbProgress([this](const std::string& url, long p, long t) {
        mdInfo() << "Progress [ " << p << "/" << t << " ] for url= " << url;
        this->progress(url, p);
    });

    d->downloader.setCbOnFinished([this](const std::string& url) {
        mdInfo() << "Request " << url << " finished";
        auto it = std::find_if(d->requests.begin(), d->requests.end(), [&url](const std::pair<std::string, std::string> &p) {
            return p.second == url;
        });
        if (it != d->requests.end() ) {
            d->requests.erase(it);
        }
    });
}

MapDownloaderDBusServer::~MapDownloaderDBusServer()
{
    std::for_each(d->requests.begin(), d->requests.end(),[this](const std::pair<std::string, std::string> &p) {
        cancel(p.first);
    });
}

void MapDownloaderDBusServer::setOutputDirectory(const std::string& path)
{
    mdInfo() << "Setting new output path " << path;
    d->downloader.setMapFileDir(path);
}

bool MapDownloaderDBusServer::download(const std::string& mapName)
{
    auto it = d->requests.find(mapName);
    if (it == d->requests.end() ) {
        const std::string url = d->downloader.download(mapName);
        d->requests[mapName] = url;
        mdInfo() << "Downloading " << mapName << " url is= " << url;
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
    auto it = d->requests.find(mapName);
    if (it != d->requests.end()) {
        d->downloader.cancel(it->second);
        d->requests.erase(it);
    } else {
        mdError() << "Unable to find " << mapName;
    }
}

} // namespace md
