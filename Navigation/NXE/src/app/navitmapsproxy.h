#ifndef NAVITMAPSPROXY_H
#define NAVITMAPSPROXY_H

#include <QObject>
#include <memory>

#include "imapdownloader.h"

namespace NXE {
struct JSONMessage;
class NXEInstance;
}

class QQmlContext;

class NavitMapsProxy : public QObject
{
    Q_OBJECT
public:
    explicit NavitMapsProxy(const std::shared_ptr<NXE::NXEInstance>& nxe, QQmlContext* ctx, QObject *parent = 0);
    ~NavitMapsProxy();

public slots:
    void downloadMap(const QString& map);
    bool isMapDownloaded(const QString& mapName);
    qreal mapSize(const QString& mapName);
    void cancelDownload(const QString& mapName);

    void reloadMaps();
signals:
    void mapDownloadError(const QString& error);
    void mapDownloadProgress(quint64 now, quint64 total, const QString& map);
    void mapDownloadFinished(const QString& map);
private:


    std::shared_ptr<NXE::NXEInstance> nxeInstance;
    QObjectList m_maps;
    QObjectList m_mapsRecommended;
    QObjectList m_mapsDownloaded;
    NXE::MapDownloaderListener mapDownloaderListener;
    std::vector<NXE::MapInfo> m_nxeMaps;
    QQmlContext* m_ctx;
    std::map<std::string, QObjectList> m_mapsByContinent;
};

#endif // NAVITMAPSPROXY_H
