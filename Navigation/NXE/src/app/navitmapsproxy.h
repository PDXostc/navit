#ifndef NAVITMAPSPROXY_H
#define NAVITMAPSPROXY_H

#include <QObject>
#include <memory>

#include "imapdownloader.h"

namespace NXE {
struct JSONMessage;
class NXEInstance;
}

class NavitMapsProxy : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObjectList maps READ maps)
public:
    explicit NavitMapsProxy(const std::shared_ptr<NXE::NXEInstance>& nxe, QObject *parent = 0);
    ~NavitMapsProxy();

    QObjectList maps() const { return m_maps;}
public slots:
    void downloadMap(const QString& map);

    bool isMapDownloaded(const QString& mapName);
signals:
    void mapDownloadError(const QString& error);
    void mapDownloadProgress(quint64 now, quint64 total, const QString& map);
    void mapDownloadFinished(const QString& map);
private:
    std::shared_ptr<NXE::NXEInstance> nxeInstance;
    QObjectList m_maps;
    NXE::MapDownloaderListener mapDownloaderListener;
    std::vector<NXE::MapInfo> m_nxeMaps;
};

#endif // NAVITMAPSPROXY_H
