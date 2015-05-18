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
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace bfs = boost::filesystem;

namespace {
const std::string mapDescFilename{ "osm_maps.xml" };
const std::string partiallyDownloadedPrefix{ ".part" };
const std::vector<std::string> mapDescriptionFilePaths{ "/usr/share/nxe/", "/home/jlr02/",
    getenv("HOME"), boost::filesystem::current_path().string(),
    boost::filesystem::current_path().string() + "/src/mapdownloader" };

} // anonymous namespace

struct MapFile {
    std::string filename;
    FILE* stream;
    MapDownloader* this_;
    const std::string url;
    std::uint32_t currentProgress;
    std::string timestamp;
};

struct MapDownloaderPrivate {
    std::thread currentThread;
    bool downloading{ false };

    std::string mapFilePath;
    std::vector<std::string> cancelRequests;
    MapDesc mdesc;

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

    bool mapDirOk() const
    {
        return bfs::exists(mapFilePath) && bfs::is_directory(mapFilePath);
    }

    bool mapDownloaded(const std::string& map) const
    {
        const bfs::path dir{ mapFilePath };
        const std::string fileName = map + ".bin";
        const bfs::path filePath{ dir / fileName };
        return bfs::exists(filePath);
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
    const std::string descFilePath = *firstPath + "/" + mapDescFilename;

    mdTrace() << "Map desc file is=" << descFilePath;
    d->mdesc.setDataFilePath(descFilePath);

    // try to create directory
    // default map dir is $HOME/.NavIt/maps
    setMapFileDir(std::string{ getenv("HOME") } + std::string{ "/.NavIt/" } + std::string{ "maps" });

    curl_global_init(CURL_GLOBAL_DEFAULT);
}

MapDownloader::~MapDownloader()
{
    curl_global_cleanup();
    mdDebug() << __PRETTY_FUNCTION__;
    if (d->currentThread.joinable()) {
        d->currentThread.join();
    }
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

size_t MapDownloader::headersWrite(void* buffer, size_t size, size_t nmemb, void* userp)
{
    char* buff = reinterpret_cast<char*>(buffer);
    struct MapFile* out = (struct MapFile*)userp;

    int res = 0;
    if (out) {
        std::string headers{ buff, size * nmemb };
        res = size * nmemb;
        mdTrace() << "Header " << headers;
        if (boost::algorithm::starts_with(headers, "Location:")) {
            boost::algorithm::erase_all(headers, "Location:");
            boost::algorithm::trim(headers);
//            mdDebug() << "Correct location is = " << headers;
            out->timestamp = headers;
        }
    }

    return res;
}

void MapDownloader::onDownloadError(const std::string& url, CURLcode err)
{
    mdDebug() << "onDownloadError: Code = " << err << " , " << curl_easy_strerror(err);

    if (cbOnError) {
        cbOnError(url, curl_easy_strerror(err));
    }
}

int MapDownloader::onDownloadProgress(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t , curl_off_t )
{
    if (dlnow == 0 || dltotal == 0) {
        //progress is 0, nothing to notify
        return CURLE_OK;
    }

    MapFile* pDownloader = reinterpret_cast<MapFile*>(clientp);

    auto vec = pDownloader->this_->d->cancelRequests;
    if (std::find(vec.begin(), vec.end(), pDownloader->url) != vec.end()) {
        mdInfo() << "Need to cancel request " << pDownloader->url;
        return 10;
    };

    const std::uint32_t progress = (static_cast<double>(dlnow * 100) / static_cast<double>(dltotal));
    if (progress != pDownloader->currentProgress) {
        mdDebug() << "Download Progress: " << progress;
        pDownloader->currentProgress = progress;

        // notify
        if (pDownloader && pDownloader->this_ && pDownloader->this_->cbOnProgress) {
            pDownloader->this_->cbOnProgress(pDownloader->url, dlnow, dltotal);
        }
    }

    return CURLE_OK;
}

long MapDownloader::getEstimatedSize(const std::string& name)
{

    boost::optional<MapData> m = d->mdesc.getMapData(name);
    if (m) {
        return m->size;
    }
    return 0;
}

std::string MapDownloader::createMapRequestString(const std::string& name, const std::string& timestamp)
{
    std::string res = "http://maps9.navit-project.org/api/map/?bbox=";

    boost::optional<MapData> m = d->mdesc.getMapData(name);
    if (m) {
        res += m->lon1 + "," + m->lat1 + "," + m->lon2 + "," + m->lat2 + timestamp;
        mdDebug() << "createMapRequestString : " << res;
        mdInfo() << "Size for map " << name << " is " << m->size / 1024 / 1024 << " MB";
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
        cbOnError(req, "");
        return req;
    }

    if (d->downloading) {
        mdInfo() << "Some map is already downloading";
        return "";
    }

    if (d->currentThread.joinable()) {
        d->currentThread.join();
    }

    auto thread = std::thread{ [req, name, this]() {
        d->downloading = true;
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
        long respCode;
        CURLcode res;
        curl = curl_easy_init();

        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, req.c_str());
            curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headersWrite);
            curl_easy_setopt(curl, CURLOPT_WRITEHEADER, &mapfile);
            curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15L);

            // This blocks
            res = curl_easy_perform(curl);
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &respCode);
            mdDebug() << "CURL res=" << respCode;

            if (respCode == 302) {
                const std::string newUrl = createMapRequestString(name,mapfile.timestamp);
                mdInfo() << "Url for " << name << " is = " << newUrl;
                curl_easy_reset(curl);
                curl_easy_setopt(curl, CURLOPT_URL, newUrl.c_str());
                curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
                curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15L);
                curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, 0);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, dataWrite);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mapfile);
                curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, onDownloadProgress);
                curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &mapfile);
                curl_easy_setopt(curl, CURLOPT_NOPROGRESS, m_reportProgess ? 0 : 1);
                res = curl_easy_perform(curl);

                if (res != CURLE_OK || respCode > 500) {
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
                    if(cbOnFinished) {
                        cbOnFinished(req);
                    }
                }
            }

            mdInfo() << "Downloading done for " << name;
            d->downloading = false;
            curl_easy_cleanup(curl);
        }
    } };

    d->currentThread = std::move(thread);
    return req;
}

std::vector<MapEntry> MapDownloader::maps() const
{
    auto maps = d->mdesc.availableMaps();
    std::vector<MapEntry> proper;
    std::for_each(maps.begin(), maps.end(), [&proper, this](const MapInfo& mi) {
        proper.emplace_back(MapEntry{mi.name, mi.size, d->mapDownloaded(mi.name), mi.continent});
    });
    mdDebug() << "Available maps size= " << maps.size();
    return proper;
}

void MapDownloader::cancel(const std::string& reqUrl)
{
    mdInfo() << "Canceling download " << reqUrl;
    if (d->downloading) {
        mdTrace() << "Request canceling ";
        auto it2 = d->cancelRequests.insert(d->cancelRequests.end(), reqUrl);
        if (d->currentThread.joinable()) {
            d->currentThread.join();
        }
        d->cancelRequests.erase(it2);
    }
}

void MapDownloader::enableReportProgress(bool flag)
{
    m_reportProgess = flag;
}

bool MapDownloader::setMapFileDir(const std::string& dir)
{
    const bfs::path mapPath{ dir };

    try {
        if (!bfs::exists(mapPath)) {
            mdDebug() << "Path " << mapPath.string() << " doesn't exists yet, try to create it";

            if (!bfs::create_directories(mapPath)) {
                mdError() << "Unable to create path " << mapPath.string() << " Caller have to properly set a path";
                return false;
            }
        }

        if (bfs::exists(mapPath) && !bfs::is_directory(mapPath)) {
            // path is not directory, but it should be
            mdError() << "Path " << mapPath.string() << " is not a directory";
            return false;
        }
    }
    catch (const std::exception& err) {
        mdError() << "An error= " << err.what() << " happened ";
        return false;
    }

    d->mapFilePath = mapPath.string();
    mdInfo() << "Setting maps dir to " << d->mapFilePath;
    return true;
}

void MapDownloader::removeMaps()
{
    bfs::remove_all(d->mapFilePath);
    setMapFileDir(d->mapFilePath);
}
