#ifndef NAVITQUICKPROXY_H
#define NAVITQUICKPROXY_H

#include <QtCore/QObject>
#include <memory>

#include "appsettings.h"

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
public:
    explicit NavitQuickProxy(const QString& socketName, QObject *parent = 0);

    int orientation();
    void setOrientation(int);

    QString version() const;
    QString position() const;

    bool enablePoi() const;
    void setEnablePoi(bool enable);

signals:
    void orientationChanged();
    void positionChanged();
    void enablePoiChanged();

public slots:
    void zoomIn();
    void zoomOut();
    void moveTo(int x, int y);
    void render();

private:
    std::shared_ptr<Context> context;
    std::shared_ptr<NXE::NXEInstance> nxeInstance;
    QString m_position;
    AppSettings m_settings;
};

#endif // NAVITQUICKPROXY_H
