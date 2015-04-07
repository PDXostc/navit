#include "mapdownloaderdbusserver.h"
#include "mapdownloader.h"
#include "mdlog.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace bu = boost::uuids;

namespace md {

struct MapDownloaderDBusServerPrivate {
    std::vector<bu::uuid> currentRequests;
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
        mdInfo() << "Progress [ " << p << "/" << t << " ] for url= " << url;;
        this->progress(url, p);
    });
}

MapDownloaderDBusServer::~MapDownloaderDBusServer()
{
    mdTrace() << __PRETTY_FUNCTION__;
}

void MapDownloaderDBusServer::setOutputDirectory(const std::string& path)
{
    mdInfo() << "Setting new output path " << path;
    d->downloader.setMapFileDir(path);
}

bool MapDownloaderDBusServer::download(const std::string& mapName)
{
    const std::string url = d->downloader.download(mapName);
    mdInfo() << "Downloading " << mapName << " url is= " << url;
    return !url.empty();
}

void MapDownloaderDBusServer::reportProgress(const bool& enable)
{
    d->downloader.enableReportProgress(enable);
}

void MapDownloaderDBusServer::cancel()
{
    d->downloader.cancel();
}

} // namespace md
