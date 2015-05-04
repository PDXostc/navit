#ifndef NAVITQUICKPROXY_H
#define NAVITQUICKPROXY_H

#include <QtCore/QObject>
#include <QtQml/QQmlListProperty>
#include <memory>

#include "appsettings.h"
#include "imapdownloader.h"
#include "navitmapsproxy.h"
#include "locationproxy.h"

namespace NXE {
struct JSONMessage;
class NXEInstance;
}
struct Context;
class QQmlContext;

class NavitQuickProxy : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(QString version READ version CONSTANT)
    Q_PROPERTY(QString position READ position NOTIFY positionChanged)
    Q_PROPERTY(bool enablePoi READ enablePoi WRITE setEnablePoi NOTIFY enablePoiChanged)
    Q_PROPERTY(bool ftu READ ftu WRITE setFtu)
    Q_PROPERTY(QObject* currentlySelectedItem READ currentlySelectedItem NOTIFY currentlySelectedItemChanged)
public:
    explicit NavitQuickProxy(const QString& socketName, QQmlContext* ctx, QObject *parent = 0);

    int orientation();
    void setOrientation(int);

    QString version() const;
    QString position() const;

    bool enablePoi() const;
    void setEnablePoi(bool enable);

    bool ftu() const;
    void setFtu(bool value);

    QObject* currentlySelectedItem() const;
    QObject* navitMapsProxy() {return &mapsProxy;}

signals:
    void orientationChanged();
    void positionChanged();
    void enablePoiChanged();

    void quitSignal();

    void searchDone();

    void currentlySelectedItemChanged();

public slots:
    void zoomIn();
    void zoomOut();
    void moveTo(int x, int y);
    void render();

    void quit();

    QString valueFor(const QString& optionName);
    void changeValueFor(const QString& optionName, const QVariant& newVal);

    void search(const QString& name);
    void setLocationPopUp(const QString& name);

private:

    void synchronizeNavit();

    std::shared_ptr<Context> context;
    std::shared_ptr<NXE::NXEInstance> nxeInstance;
    QQmlContext* m_rootContext;
    QString m_position;
    AppSettings m_settings;
    NavitMapsProxy mapsProxy;
    QObjectList m_searchResults;
    LocationProxy* m_currentItem {nullptr};
};

#endif // NAVITQUICKPROXY_H
