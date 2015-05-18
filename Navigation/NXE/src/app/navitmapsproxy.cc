#include "navitmapsproxy.h"
#include "nxe_instance.h"
#include "mapinfoproxy.h"
#include "alog.h"

#include <QtQml/QQmlContext>

NavitMapsProxy::NavitMapsProxy(const std::shared_ptr<NXE::NXEInstance> &nxe, QQmlContext *ctx, QObject *parent)
    : QObject(parent)
    , nxeInstance(nxe)
    , m_ctx(ctx)
{
    // mapDownloaderCallbacks!
    mapDownloaderListener.progressCb = [this](const std::string& mapName, std::uint64_t now, std::uint64_t total) {
        emit mapDownloadProgress(now, total, QString::fromStdString(mapName));
    };
    mapDownloaderListener.errorCb = [this](const std::string&, const std::string& strError) {
        emit mapDownloadError(QString::fromStdString(strError));
    };
    mapDownloaderListener.finishedCb = [this](const std::string& map) {
        reloadMaps();
        emit mapDownloadFinished(QString::fromStdString(map));
    };

    nxeInstance->setMapDownloaderListener(mapDownloaderListener);

    reloadMaps();
}

NavitMapsProxy::~NavitMapsProxy()
{
    qDeleteAll(m_maps);
    m_maps.clear();
}

void NavitMapsProxy::downloadMap(const QString& map)
{
    try {
        nxeInstance->mapDownloader()->download(map.toStdString());
    }
    catch (const std::exception& ex) {
        // this may throw if MapDownloader is not available
        emit mapDownloadError("MapDownloader DBus service is probably not running");
    }
}

bool NavitMapsProxy::isMapDownloaded(const QString &mapName)
{
    return std::find_if(m_nxeMaps.begin(), m_nxeMaps.end(), [&mapName](const NXE::MapInfo& mi) -> bool{
        return mi.name == mapName.toStdString() && mi.downloaded;
    }) != m_nxeMaps.end();
}

qreal NavitMapsProxy::mapSize(const QString &mapName)
{
    auto it = std::find_if(m_nxeMaps.begin(), m_nxeMaps.end(), [&mapName](const NXE::MapInfo& mi) -> bool{
        return mi.name == mapName.toStdString() && mi.downloaded;
    });

    if ( it != m_nxeMaps.end() ) {

        qreal ret = (it != m_nxeMaps.end() ? it->size/(1024 * 1024) : -1);
        aTrace() << "Map size for " << mapName.toStdString() << " is " << ret;
        return ret;
    } else {
        aError() << "Unable to find map size for " << mapName.toStdString();
        return 0;
    }
}

void NavitMapsProxy::reloadMaps()
{
    qDeleteAll(m_maps);
    m_maps.clear();

    // Request for available maps
    m_nxeMaps = nxeInstance->mapDownloader()->maps();
    std::sort(m_nxeMaps.begin(), m_nxeMaps.end(), [] (const NXE::MapInfo& lhs, const NXE::MapInfo& rhs) ->bool {
        return lhs.name < rhs.name;
    });
    std::for_each(m_nxeMaps.begin(), m_nxeMaps.end(), [this](const NXE::MapInfo& mi) {
        m_maps.append(new MapInfoProxy{mi});
    });

    m_ctx->setContextProperty("allMapsModel", QVariant::fromValue(m_maps));
}
