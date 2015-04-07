//============================================================================
// Name        : mapdownloader.cpp
// Author      :
// Version     :
// Copyright   :
// Description : Navit Map downloader
//============================================================================

#include <iostream>
#include <thread>
#include <map>
#include "mapdownloader.h"
#include "mapdesc.h"
#include "mdlog.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

namespace bfs = boost::filesystem;

namespace {
const std::string writeErrorStr{ "Write error" };
const std::string timeoutErrorStr{ "Timeout error" };
const std::string connectionErrorStr{ "Connection error" };
const std::string mapErrorStr{ "Map error" };
const std::string unknownErrorStr{ "Unknown error" };
const std::string mapDescFilename{ "osm_maps.xml" };
const std::string partiallyDownloadedPrefix{ ".part" };
const std::vector<std::string> mapDescriptionFilePaths{ "/usr/share/nxe/", "/home/jlr02/",
    getenv("HOME"), boost::filesystem::current_path().string(),
    boost::filesystem::current_path().string() + "/src/mapdownloader" };

enum class Status {
    idle = 0,
    downloading,
    canceling,
    stopped
};
} // anonymous namespace

struct MapFile {
    std::string filename;
    FILE* stream;
    MapDownloader* this_;
    const std::string url;
    std::uint32_t currentProgress;
};

struct MapDownloaderPrivate {
    std::map<std::string, std::unique_ptr<std::thread> > urlThreads;
    Status status;
    std::string mapFilePath;
    std::string mapDescFilePath;

    void prepareFile(const std::string& fileName)
    {
        const bfs::path mapFilePath{ fileName };
        const bfs::path mapFilePartPath{ fileName + partiallyDownloadedPrefix };
        if (bfs::exists(mapFilePath)) {
            mdInfo() << "File " << mapFilePath.string() << " exists, remove before downloading";
            bfs::remove(mapFilePath);
        }

        if (bfs::exists(mapFilePartPath)) {
            mdInfo() << "File " << mapFilePartPath.string() << " exists, remove before downloading";
            bfs::remove(mapFilePartPath);
        }
    }
};

MapDownloader::MapDownloader()
    : d(new MapDownloaderPrivate)
    , m_reportProgess(true)
{
    auto firstPath = std::find_if(std::begin(mapDescriptionFilePaths), std::end(mapDescriptionFilePaths), [this](const std::string& path) -> bool {
            const bfs::path bPath {path + std::string {"/"} + mapDescFilename};
            mdTrace() << "Looking for " << mapDescFilename << " in " << path;
            // check if file exists
            return bfs::exists(bPath);
    });

    if (firstPath == mapDescriptionFilePaths.end()) {
        mdError() << "Unable to find osm_maps.xml";
        throw std::runtime_error("Unable to find osm_maps.xml");
    }
    mdInfo() << "Reading " << mapDescFilename << " from " << *firstPath;
    d->mapDescFilePath = *firstPath + "/" + mapDescFilename;
    mdTrace() << "Map desc file is=" << d->mapDescFilePath;

    // try to create directory
    // default map dir is $HOME/maps
    setMapFileDir(std::string{ getenv("HOME") } + std::string{ "/" } + std::string{ "maps" });

    curl_global_init(CURL_GLOBAL_DEFAULT);
    d->status = Status::idle;
}

MapDownloader::~MapDownloader()
{
    cancel();
    curl_global_cleanup();
    mdDebug() << __PRETTY_FUNCTION__;
}

size_t MapDownloader::dataWrite(void* buffer, size_t size, size_t nmemb, void* stream)
{
    struct MapFile* out = (struct MapFile*)stream;
    if (out && !out->stream) {
        /* open file for writing */
        out->stream = fopen(out->filename.c_str(), "wb");
        if (!out->stream) {
            out->this_->onDownloadError(out->url, CURLE_WRITE_ERROR);
            return -1; /* failure, can't open file to write */
        }
    }
    return fwrite(buffer, size, nmemb, out->stream);
}

std::string MapDownloader::getDownloadErrorStr(CURLcode err)
{
    switch (err) {
    case CURLE_WRITE_ERROR:
        return writeErrorStr;

    case CURLE_OPERATION_TIMEDOUT:
        return timeoutErrorStr;

    case CURLE_COULDNT_CONNECT:
        return connectionErrorStr;

    default:
        return unknownErrorStr;
    }
    return unknownErrorStr;
}

void MapDownloader::onDownloadError(const std::string& url, CURLcode err)
{
    mdDebug() << "onDownloadError: Code = " << err << " , " << curl_easy_strerror(err);

    if (cbOnError) {
        cbOnError(url, getDownloadErrorStr(err));
    }
}

int MapDownloader::onDownloadProgress(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
    if (dlnow == 0 || dltotal == 0) {
        //progress is 0, nothing to notify
    }
    else {
        MapFile* pDownloader = reinterpret_cast<MapFile*>(clientp);
        if (pDownloader->this_->d->status == Status::canceling) {
            pDownloader->this_->d->status = Status::stopped;
            return 10;
        }
        const std::uint32_t progress = (static_cast<double>(dlnow * 100) / static_cast<double>(dltotal));
        if (progress != pDownloader->currentProgress) {
            mdDebug() << "Download Progress: " << progress;
            pDownloader->currentProgress = progress;

            // notify
            if (pDownloader && pDownloader->this_ && pDownloader->this_->cbOnProgress) {
                pDownloader->this_->cbOnProgress(pDownloader->url, dlnow, dltotal);
            }
        }
    }

    return CURLE_OK;
}

long MapDownloader::getEstimatedSize(const std::string& name)
{
    MapDesc mdesc;
    MapData m;

    if (mdesc.getMapData(name, d->mapDescFilePath, m)) {
        return m.size;
    }
    return 0;
}

std::string MapDownloader::createMapRequestString(const std::string& name)
{
    MapDesc mdesc;
    MapData m;

    std::string res = "http://maps5.navit-project.org/api/map/?bbox=";

    if (mdesc.getMapData(name, d->mapDescFilePath, m)) {
        res += m.lon1 + "," + m.lat1 + "," + m.lon2 + "," + m.lat2 + "&timestamp=150320";
        mdDebug() << "createMapRequestString : " << res;
    }
    else {
        mdError() << "Unable to get address for " << name;
        res = "";
    }

    return res;
}

std::string MapDownloader::download(const std::string& name)
{
    const std::string req{ createMapRequestString(name) };

    if (req.empty()) {
        cbOnError(req, mapErrorStr);
        return req;
    }

    if (d->urlThreads.find(req) != d->urlThreads.end()) {
        mdInfo() << "A url " << req << " is already being downloaded";
        return "";
    }

    d->urlThreads[req] = std::unique_ptr<std::thread>{ new std::thread{ [req, name, this]() {
        const std::string mapFileName{ d->mapFilePath + std::string {"/"} + name + std::string{".bin"} };
        d->prepareFile(mapFileName);
        mdDebug() << "Download starting. Url= " << req << " result filename= " << mapFileName;

        MapFile mapfile = {
            mapFileName + partiallyDownloadedPrefix,
            nullptr,
            this,
            req,
            0
        };

        CURL* curl{ nullptr };
        CURLcode res;
        curl = curl_easy_init();

        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, req.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, dataWrite);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mapfile);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, onDownloadProgress);
            curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &mapfile);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, m_reportProgess ? 0 : 1);

            // This blocks
            d->status = Status::downloading;
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);

            if (res != CURLE_OK ) {
                onDownloadError(req, res);
            } else {
                const bfs::path newPath { mapFileName };
                const bfs::path oldPath { mapfile.filename };
                mdInfo() << "Renaming from " << oldPath.string() << " to " << newPath.string();
                bfs::rename(oldPath, newPath);
                // Download properly finished
                if (mapfile.stream) {
                    fclose(mapfile.stream);
                }
                mdInfo() << " Downloading " << req << " finished";
            }
        }
    } } };

    return req;
}

void MapDownloader::cancel()
{
    mdInfo() << "Canceling all downloads";
    d->status = Status::canceling;
    std::for_each(d->urlThreads.begin(),
        d->urlThreads.end(),
        [this](std::pair<const std::string, std::unique_ptr<std::thread> >& t) {
        mdDebug() << "Waiting for " << t.first << " to finish";
        t.second->join();
        });

    d->urlThreads.clear();
}

void MapDownloader::enableReportProgress(bool flag)
{
    m_reportProgess = flag;
}

void MapDownloader::setMapFileDir(const std::string& dir)
{
    const bfs::path mapPath{ dir };

    if (!bfs::exists(mapPath)) {
        mdDebug() << "Path " << mapPath.string() << " doesn't exists yet, try to create it";

        if (!bfs::create_directories(mapPath)) {
            mdError() << "Unable to create path " << mapPath.string() << " Caller have to properly set a path";
            return;
        }
    }

    if (bfs::exists(mapPath) && !bfs::is_directory(mapPath)) {
        // path is not directory, but it should be
        mdError() << "Path " << mapPath.string() << " is not a directory";
        return;
    }

    d->mapFilePath = mapPath.string();
}

void MapDownloader::setMapDescFilePath(const std::string& path)
{
    d->mapDescFilePath = path;
}

bool MapDownloader::mapDirOk() const
{
    return bfs::exists(d->mapFilePath) && bfs::is_directory(d->mapFilePath);
}
