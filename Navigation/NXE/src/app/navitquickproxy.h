#ifndef NAVITQUICKPROXY_H
#define NAVITQUICKPROXY_H

#include <QtCore/QObject>
#include <QtCore/QRect>
#include <QtQml/QQmlListProperty>
#include <memory>

#include "appsettings.h"
#include "imapdownloader.h"
#include "navitmapsproxy.h"
#include "locationproxy.h"
#include "navitnavigationproxy.h"

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
    Q_PROPERTY(bool enablePoi READ enablePoi WRITE setEnablePoi NOTIFY enablePoiChanged)
    Q_PROPERTY(bool ftu READ ftu WRITE setFtu NOTIFY ftuChanged)
    Q_PROPERTY(QObject* currentlySelectedItem READ currentlySelectedItem NOTIFY currentlySelectedItemChanged)
    Q_PROPERTY(QObject* waypointItem READ waypointItem NOTIFY waypointItemChanged)

public:
    NavitQuickProxy(const QString& socketName, QQmlContext* ctx, QObject *parent = 0);
    ~NavitQuickProxy();

    int orientation();
    void setOrientation(int);

    QString version() const;
    QString position() const;

    bool enablePoi() const;
    void setEnablePoi(bool enable);

    bool ftu() const;
    void setFtu(bool value);

    QObject* waypointItem() const;
    QObject* currentlySelectedItem() const;
    QObject* navitMapsProxy() {return &mapsProxy;}
    QObject* navitNavigationProxy() {return &navigationProxy;}

    void resize(const QRect& rect);
signals:
    void orientationChanged();
    void enablePoiChanged();
    void ftuChanged();
    void quitSignal();
    void searchDone();
    void gettingHistoryDone();
    void currentlySelectedItemChanged();
    void waypointItemChanged();
    void topBarLocationVisibleChanged();

    void reloadQueue(const QString&, const QObjectList& l);

public slots:
    void render();
    void reset();
    void quit();

    QString valueFor(const QString& optionName);
    void changeValueFor(const QString& optionName, const QVariant& newVal);

    // Search API
    void startSearch();
    void finishSearch();
    void searchCountry(const QString& name);
    void searchCity(const QString& name);
    void searchStreet(const QString& street);
    void searchAddress(const QString& street);
    void searchSelect(const QString& what, int id);
    void searchNear(const QString& str);
    void moveToCurrentPosition();
    void getFavorites();
    void getHistory();
    void setLocationPopUp(const QUuid& id);
    void setZoom(int newZoom);

    void clearWaypoint();

private slots:
    void initNavit();
    void synchronizeNavit();

    void reloadQueueSlot(const QString& listName, const QObjectList& list);
private:

    std::shared_ptr<Context> context;
    std::shared_ptr<NXE::NXEInstance> nxeInstance;
    QQmlContext* m_rootContext;
    AppSettings m_settings;
    NavitMapsProxy mapsProxy;
    NavitNavigationProxy navigationProxy;
    QObjectList m_countriesSearchResults;
    QObjectList m_citiesSearchResults;
    QObjectList m_streetsSearchResults;
    QObjectList m_addressSearchResults;
    QObjectList m_favoritesResults;
    QObjectList m_historyResults;
    QScopedPointer<LocationProxy> m_currentItem;
    QScopedPointer<LocationProxy> m_waypointItem;
    bool m_ignoreNextClick;
};

#endif // NAVITQUICKPROXY_H
