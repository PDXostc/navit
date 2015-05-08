#ifndef NAVITQUICKPROXY_H
#define NAVITQUICKPROXY_H

#include <QtCore/QObject>
#include <QtCore/QRect>
#include <QtQml/QQmlListProperty>
#include <memory>

// This include has to be here
// in order for boost mpl to correctly mark
// boost::mpl::vector size
#include "calls.h"

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
    Q_PROPERTY(bool topBarLocationVisible READ topBarLocationVisible WRITE setTopBarLocationVisible NOTIFY topBarLocationVisibleChanged)

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

    bool topBarLocationVisible() const;
    void setTopBarLocationVisible(bool value);

    void resize(const QRect& rect);

signals:
    void orientationChanged();
    void positionChanged();
    void enablePoiChanged();

    void quitSignal();

    void searchDone();
    void gettingFavoritesDone();
    void gettingHistoryDone();
    void currentlySelectedItemChanged();
    void topBarLocationVisibleChanged();

    void pointClicked(LocationProxy* location);

public slots:
    void zoomIn();
    void zoomOut();
    void moveTo(int x, int y);
    void render();

    void quit();

    QString valueFor(const QString& optionName);
    void changeValueFor(const QString& optionName, const QVariant& newVal);

    void startSearch();
    void finishSearch();
    void searchCountry(const QString& name);
    void searchCity(const QString& name);
    void getFavorites();
    void getHistory();
    void setLocationPopUp(const QString& name);
    void hideLocationBars();
    void setTopBarVisibility(const bool value);

private slots:
    void initNavit();
    void synchronizeNavit();
private:

    std::shared_ptr<Context> context;
    std::shared_ptr<NXE::NXEInstance> nxeInstance;
    QQmlContext* m_rootContext;
    QString m_position;
    AppSettings m_settings;
    NavitMapsProxy mapsProxy;
    QObjectList m_countriesSearchResults;
    QObjectList m_citiesSearchResults;
    QObjectList m_favoritesResults;
    QObjectList m_historyResults;
    QScopedPointer<LocationProxy> m_currentItem;
};

#endif // NAVITQUICKPROXY_H
