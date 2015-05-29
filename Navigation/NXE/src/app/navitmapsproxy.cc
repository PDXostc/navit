#include "navitmapsproxy.h"
#include "nxe_instance.h"
#include "mapinfoproxy.h"
#include "alog.h"

#include <QtQml/QQmlContext>

NavitMapsProxy::NavitMapsProxy(const std::shared_ptr<NXE::NXEInstance>& nxe, QQmlContext* ctx, QObject* parent)
    : QObject(parent)
    , nxeInstance(nxe)
    , m_ctx(ctx)
    , m_reloadThreadRunning(false)
{

    connect(this,&NavitMapsProxy::_updateMapsModel, this, &NavitMapsProxy::updateModels, Qt::QueuedConnection);

    // mapDownloaderCallbacks!
    mapDownloaderListener.progressCb = [this](std::string mapName, std::uint64_t now, std::uint64_t total) {
        emit mapDownloadProgress(now, total, QString::fromStdString(mapName));
    };
    mapDownloaderListener.errorCb = [this](std::string, std::string strError) {
        emit mapDownloadError(QString::fromStdString(strError));
    };
    mapDownloaderListener.finishedCb = [this](std::string map) {
        emit mapDownloadFinished(QString::fromStdString(map));
    };

    nxeInstance->setMapDownloaderListener(mapDownloaderListener);
}

NavitMapsProxy::~NavitMapsProxy()
{

    if (m_reloadMapsThread.joinable()) {
        m_reloadMapsThread.join();
    }

    qDeleteAll(m_maps);
    qDeleteAll(m_mapsRecommended);
    qDeleteAll(m_mapsDownloaded);
    m_maps.clear();
    m_mapsRecommended.clear();
    m_mapsDownloaded.clear();
    for (auto iter = m_mapsByContinent.begin(); iter != m_mapsByContinent.end(); ++iter)
        qDeleteAll(iter->second);
    m_mapsByContinent.clear();
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

bool NavitMapsProxy::isMapDownloaded(const QString& mapName)
{
    return std::find_if(m_nxeMaps.begin(), m_nxeMaps.end(), [&mapName](const NXE::MapInfo& mi) -> bool {
        return mi.name == mapName.toStdString() && mi.downloaded;
    }) != m_nxeMaps.end();
}

qreal NavitMapsProxy::mapSize(const QString& mapName)
{
    auto it = std::find_if(m_nxeMaps.begin(), m_nxeMaps.end(), [&mapName](const NXE::MapInfo& mi) -> bool {
        return mi.name == mapName.toStdString();
    });

    if (it != m_nxeMaps.end()) {

        qreal ret = (it != m_nxeMaps.end() ? it->size / (1024 * 1024) : -1);
        aTrace() << "Map size for " << mapName.toStdString() << " is " << ret << " raw = " << it->size;
        return it->size;
    }
    else {
        aError() << "Unable to find map size for " << mapName.toStdString();
        return 0;
    }
}

void NavitMapsProxy::cancelDownload(const QString& mapName)
{
    nxeInstance->mapDownloader()->cancel(mapName.toStdString());
}

void NavitMapsProxy::requestMapsReload()
{
    aDebug() << "Requesting maps reload";
    if (m_reloadThreadRunning) {
        aInfo() << "Reload already in progress";
        return;
    }

    m_ctx->setContextProperty("allMapsModel", QVariant::fromValue(QObjectList{}));
    qDeleteAll(m_maps);
    m_maps.clear();

    m_ctx->setContextProperty("recommendedMapsModel", QVariant::fromValue(QObjectList{}));
    qDeleteAll(m_mapsRecommended);
    m_mapsRecommended.clear();

    m_ctx->setContextProperty("downloadedMapsModel", QVariant::fromValue(QObjectList{}));
    qDeleteAll(m_mapsDownloaded);
    m_mapsDownloaded.clear();

    NXE::MapInfo downloaded = { "Downloaded Maps", 0, false, "" };
    NXE::MapInfo recommended = { "Recommended Maps", 0, false, "" };
    NXE::MapInfo earth = { "Earth", 0, false, "Earth" };
    auto p = new MapInfoProxy{ downloaded };
    p->moveToThread(m_ctx->thread());
    m_maps.append(p);

    p = new MapInfoProxy{ recommended };
    p->moveToThread(m_ctx->thread());
    m_maps.append(p);

    p = new MapInfoProxy{ earth };
    p->moveToThread(m_ctx->thread());
    m_maps.append(p);

    for (auto iter = m_mapsByContinent.begin(); iter != m_mapsByContinent.end(); ++iter)
        qDeleteAll(iter->second);
    m_mapsByContinent.clear();

    m_reloadThreadRunning = true;
    m_reloadMapsThread = std::thread{ [this]() {
        reloadMaps();
    }};
}

void NavitMapsProxy::reloadMaps()
{

    aInfo() << "Reloading maps";
    // Request for available maps
    m_nxeMaps = nxeInstance->mapDownloader()->maps();
    auto pos = nxeInstance->gps()->position();

    aInfo() << "POSITION.... LON:  " << pos.longitude << " LAT: " << pos.latitude;
    if(!(std::isnan(pos.longitude) || std::isnan(pos.latitude))) {
        m_recommended = nxeInstance->mapDownloader()->recommendedMaps(pos.longitude, pos.latitude);
    }

    emit _updateMapsModel();
}

void NavitMapsProxy::updateModels()
{
    m_reloadThreadRunning = false;
    if(m_reloadMapsThread.joinable()) {
        m_reloadMapsThread.join();
    }
    aInfo() << "Updating models";
    std::for_each(m_recommended.begin(), m_recommended.end(), [this](const NXE::MapInfo& mi) {
        m_mapsRecommended.append(new MapInfoProxy{mi});
    });

    std::sort(m_nxeMaps.begin(), m_nxeMaps.end(), [] (const NXE::MapInfo& lhs, const NXE::MapInfo& rhs) ->bool {
        return lhs.name < rhs.name;
    });
    std::for_each(m_nxeMaps.begin(), m_nxeMaps.end(), [this](const NXE::MapInfo& mi) {
        if(mi.name != mi.continent) {
            auto p = new MapInfoProxy{mi};
            p->moveToThread(m_ctx->thread());
            m_mapsByContinent[mi.continent].append(p);
            if(mi.downloaded) {
                p = new MapInfoProxy{mi};
                p->moveToThread(m_ctx->thread());
                m_mapsDownloaded.append(p);
            }
        }
        else {
            if (mi.continent != "Australia+Oceania" && mi.name != "Whole Planet") {
                 m_maps.append(new MapInfoProxy{mi});
            }
            else if (mi.name == "Whole Planet") {
                auto p = new MapInfoProxy{mi};
                p->moveToThread(m_ctx->thread());
                m_mapsByContinent["Whole Planet"].append(p);
            }
        }
    });
    m_ctx->setContextProperty("africaModel", QVariant::fromValue(m_mapsByContinent["Africa"]));
    m_ctx->setContextProperty("asiaModel", QVariant::fromValue(m_mapsByContinent["Asia"]));
    m_ctx->setContextProperty("australiaModel", QVariant::fromValue(m_mapsByContinent["Australia"]));
    m_ctx->setContextProperty("europeModel", QVariant::fromValue(m_mapsByContinent["Europe"]));
    m_ctx->setContextProperty("northAmericaModel", QVariant::fromValue(m_mapsByContinent["North America"]));
    m_ctx->setContextProperty("southMiddleAmericaModel", QVariant::fromValue(m_mapsByContinent["South+Middle America"]));
    m_ctx->setContextProperty("earthModel", QVariant::fromValue(m_mapsByContinent["Whole Planet"]));
    m_ctx->setContextProperty("downloadedMapsModel", QVariant::fromValue(m_mapsDownloaded));
    m_ctx->setContextProperty("recommendedMapsModel", QVariant::fromValue(m_mapsRecommended));
    m_ctx->setContextProperty("allMapsModel", QVariant::fromValue(m_maps));

    emit mapsReloaded();

}
