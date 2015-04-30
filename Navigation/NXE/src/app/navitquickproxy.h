#ifndef NAVITQUICKPROXY_H
#define NAVITQUICKPROXY_H

#include <QtCore/QObject>
#include <memory>

#include "appsettings.h"
#include "imapdownloader.h"

namespace NXE {
struct JSONMessage;
class NXEInstance;
}
struct Context;

class NavitQuickProxy : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(QString version READ version CONSTANT)
    Q_PROPERTY(QString position READ position NOTIFY positionChanged)
    Q_PROPERTY(bool enablePoi READ enablePoi WRITE setEnablePoi NOTIFY enablePoiChanged)
    Q_PROPERTY(bool ftu READ ftu WRITE setFtu)
public:
    explicit NavitQuickProxy(const QString& socketName, QObject *parent = 0);

    int orientation();
    void setOrientation(int);

    QString version() const;
    QString position() const;

    bool enablePoi() const;
    void setEnablePoi(bool enable);

    bool ftu() const;
    void setFtu(bool value);

signals:
    void orientationChanged();
    void positionChanged();
    void enablePoiChanged();

    void quitSignal();

    void mapDownloadError(const QString& error);

    void mapDownloadProgress(quint64 now, quint64 total, const QString& map);
    void mapDownloadFinished(const QString& map);

public slots:
    void zoomIn();
    void zoomOut();
    void moveTo(int x, int y);
    void render();

    void quit();

    QString valueFor(const QString& optionName);
    void changeValueFor(const QString& optionName, const QVariant& newVal);

    void downloadMap(const QString& map);

private:

    void synchronizeNavit();

    std::shared_ptr<Context> context;
    std::shared_ptr<NXE::NXEInstance> nxeInstance;
    QString m_position;
    AppSettings m_settings;
    NXE::MapDownloaderListener mapDownloaderListener;
};

#endif // NAVITQUICKPROXY_H
