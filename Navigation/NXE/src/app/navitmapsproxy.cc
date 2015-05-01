#include "navitmapsproxy.h"
#include "nxe_instance.h"
#include "mapinfoproxy.h"

NavitMapsProxy::NavitMapsProxy(const std::shared_ptr<NXE::NXEInstance> &nxe, QObject *parent)
    : QObject(parent)
    , nxeInstance(nxe)
{
    // mapDownloaderCallbacks!
    mapDownloaderListener.progressCb = [this](const std::string& mapName, std::uint64_t now, std::uint64_t total) {
        emit mapDownloadProgress(now, total, QString::fromStdString(mapName));
    };
    mapDownloaderListener.errorCb = [this](const std::string&, const std::string& strError) {
        emit mapDownloadError(QString::fromStdString(strError));
    };
    mapDownloaderListener.finishedCb = [this](const std::string& map) {
        emit mapDownloadFinished(QString::fromStdString(map));
    };

    nxeInstance->setMapDownloaderListener(mapDownloaderListener);

    // Request for available maps
    auto maps = nxeInstance->HandleMessage<MapsMessageTag>();
    std::for_each(maps.begin(), maps.end(), [this](const NXE::MapInfo& mi) {
        m_maps.append(new MapInfoProxy{mi});
    });
}

NavitMapsProxy::~NavitMapsProxy()
{
    qDeleteAll(m_maps);
    m_maps.clear();
}

void NavitMapsProxy::downloadMap(const QString& map)
{
    try {
        nxeInstance->HandleMessage<DownloadMessageTag>(map.toStdString());
    }
    catch (const std::exception& ex) {
        // this may throw if MapDownloader is not available
        emit mapDownloadError("MapDownloader DBus service is probably not running");
    }
}
