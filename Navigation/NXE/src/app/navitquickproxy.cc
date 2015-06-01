#include "navitquickproxy.h"
#include "nxe_instance.h"
#include "alog.h"
#include "log.h"
#include "navitdbus.h"
#include "navitprocessimpl.h"
#include "gpsdprovider.h"
#include "mapdownloaderdbus.h"
#include "dbuscontroller.h"
#include "speechimpldbus.h"
#include "nxe_version.h"
#include "mapinfoproxy.h"
#include "locationproxy.h"

#include <functional>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <QtCore/QVariant>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtQml>
#include <QtCore/QTimer>
#include <QtCore/QRect>

struct Context {
    NXE::DBusController dbusController;
    NXE::INavitIPC* ipc{ new NXE::NavitDBus{ dbusController } };
    NXE::IMapDownloader* md{ new NXE::MapDownloaderDBus{ dbusController } };
#if defined(NXE_OS_LINUX)
    NXE::ISpeech* speech{ nullptr };
#elif defined(NXE_OS_TIZEN)
    NXE::ISpeech* speech{ new NXE::SpeechImplDBus{ dbusController } };
#endif
    NXE::DI::Injector injector{ std::make_tuple(
        std::shared_ptr<NXE::INavitIPC>(ipc),
        std::shared_ptr<NXE::INavitProcess>(new NXE::NavitProcessImpl),
        std::shared_ptr<NXE::IGPSProvider>(new NXE::GPSDProvider),
        std::shared_ptr<NXE::IMapDownloader>(md),
        std::shared_ptr<NXE::ISpeech>(speech)) };
};

void clearList(QObjectList& l, const QString& modelName, QQmlContext* c)
{
    QObjectList tmp = l;
    l.clear();
    c->setContextProperty(modelName, QVariant::fromValue(l));
    qDeleteAll(tmp);
}

NavitQuickProxy::NavitQuickProxy(const QString& socketName, QQmlContext* ctx, QObject* parent)
    : QObject(parent)
    , context(new Context)
    , nxeInstance(new NXE::NXEInstance{ context->injector })
    , m_rootContext(ctx)
    , mapsProxy(nxeInstance, ctx)
    , navigationProxy(nxeInstance)
{
    nxeInstance->setWaylandSocketName(socketName.toLatin1().data());

    connect(this, &NavitQuickProxy::reloadQueue, this, &NavitQuickProxy::reloadQueueSlot, Qt::QueuedConnection);

    connect(&navigationProxy, &NavitNavigationProxy::navigationChanged, [this]() {
        //
        aDebug() << "Navigation changed to " << (navigationProxy.navigation() ? " navi " : "no-navi");
        if (navigationProxy.navigation()) {
            // find in m_history first
            LocationProxy* p = qobject_cast<LocationProxy*>(navigationProxy.currentNaviItem());
            tryToAddToHistory(p);
        } else {
            aDebug() << "Navigation canceled, change current item";

            // oh clear all items
            if(navigationProxy.currentNaviItem()) {
                LocationProxy *pp = qobject_cast<LocationProxy*>(navigationProxy.currentNaviItem());
                changeCurrentItem(LocationProxy::clone(pp));
            }
        }
    });

    nxeInstance->pointClickedSignal().connect([this](const NXE::PointClicked& pc) {

        QString name, description;
        int distance = -1;

        // We may have more than one entry on click (town, street and some poi)
        // The order of checking is (the least important is checked sooner)
        // 1.) street
        // 2.) poi
        // 3.) town

        auto fillItem = [&pc, &name, &description, &distance](const std::string& searchString) {
            auto iter = std::find_if(pc.items.begin(), pc.items.end(), [&searchString](const NXE::PointClicked::Info& p) -> bool {
                return boost::algorithm::starts_with(p.type, searchString);
            });
            if (iter != pc.items.end()) {
                name = QString::fromStdString(iter->label);
                description = QString::fromStdString(iter->address);
                distance = iter->distance;
            }
        };

        fillItem("street_");
        fillItem("cycleway");
        fillItem("poly_");
        fillItem("highway_");
        fillItem("poi_");
        fillItem("house_");
        fillItem("town_label_");

        if (name.isEmpty()) {
            aError() << "Name of clicked item cannot be empty pc= " << pc;
            return;
        }

        // check if we have proper position
        auto pos = nxeInstance->gps()->position();
        if (std::isnan(pos.longitude) || std::isnan(pos.latitude)) {
            aError() << "We don't have correct position so probably gps is not running";
            distance = -1;
        }

        // if we're navigating use different item

        nxeInstance->ipc()->setTracking(false);

        aDebug() << "Name = " << name.toStdString() << " position = " << pc.position.longitude << " " << pc.position.latitude
                 << " distance = " << distance;
        auto loc = new LocationProxy { name, false, description, false, -1, distance};
        // move to parent thread
        loc->setPosition(pc.position);
        loc->moveToThread(this->thread());
        if (navigationProxy.navigation()) {
            aInfo() << "Navigation take place, this will be an waypoint";
            setWaypointItem(loc);
        }
        else {
            aInfo() << "Not navigating, select this and show on the screen";
            changeCurrentItem(loc);
        }
    });

    nxeInstance->ipc()->tapSignal().connect([this](const NXE::PointClicked& p) {
        aTrace() << "Tap click";
        // even if it's in navigation, we allow free map mode
        nxeInstance->ipc()->setTracking(false);

        if(navigationProxy.navigation()) {
            // hey dude, don't dismiss navigation!
            return;
        }

        aDebug() << "User tapped, dismiss location bar";
        changeCurrentItem(nullptr);
    });


    // Navit dbus responses
    nxeInstance->ipc()->searchResponse().connect([this](NXE::SearchResults results, NXE::INavitIPC::SearchType type) {
        if (type == NXE::INavitIPC::SearchType::Country) {
            for (NXE::SearchResult result : results) {
                aTrace() << "result " << result.country.name;
                m_countriesSearchResults.append(new LocationProxy{ result });
            }
            aDebug() << "Country Model size = " << m_countriesSearchResults.size() << static_cast<void*>(m_rootContext);
            emit reloadQueue("countrySearchResult", m_countriesSearchResults);
        }
        else if (type == NXE::INavitIPC::SearchType::City) {
            for (NXE::SearchResult city : results) {
                m_citiesSearchResults.append(new LocationProxy{ city });
            }
            aDebug() << "City Model size = " << m_citiesSearchResults.size();
            emit reloadQueue("citySearchResult", m_citiesSearchResults);
        }
        else if (type == NXE::INavitIPC::SearchType::Street) {

            for (NXE::SearchResult street : results) {
                m_streetsSearchResults.append(new LocationProxy{ street });
            }
            aDebug() << "Street Model size = " << m_streetsSearchResults.size();
            emit reloadQueue("streetSearchResult", m_streetsSearchResults);
        }
        else if(type == NXE::INavitIPC::SearchType::Address ){
            for (NXE::SearchResult result : results) {
                m_addressSearchResults.append(new LocationProxy{ result });
            }
            aDebug() << "Address Model size = " << m_addressSearchResults.size();
            emit reloadQueue("addressSearchResult", m_addressSearchResults);
        }
        emit searchDone();
    });

    nxeInstance->ipc()->possibleTrackInfoSignal().connect([this](std::pair<std::int32_t, std::int32_t> p){
        if (m_currentItem) {
            m_currentItem->setDistance(p.first);
            auto eta = p.second;
            time_t _eta = time(NULL) + eta/10;
            auto remainingEta = _eta - time(NULL);
            m_currentItem->setEta(remainingEta);
            aTrace() << "Informations for " << m_currentItem->itemText().toStdString()
                     << " distance = " << p.first << " time = " << remainingEta;
        }
    });

    qRegisterMetaType<QObjectList>("QObjectList");
    typedef QQmlListProperty<LocationProxy> LocationProxyList;
    qRegisterMetaType<LocationProxyList>("QQmlListProperty<LocationProxy>");

    QTimer::singleShot(500, this, SLOT(initNavit()));
}

NavitQuickProxy::~NavitQuickProxy()
{
    aDebug() << __PRETTY_FUNCTION__;
    nxeInstance->setPositionUpdateListener(0);

    clearList(m_countriesSearchResults, "countrySearchResult", m_rootContext);
    clearList(m_citiesSearchResults, "citySearchResult", m_rootContext);
    clearList(m_streetsSearchResults, "streetSearchResult", m_rootContext);
    clearList(m_addressSearchResults, "addressSearchResult", m_rootContext);
    clearList(m_favoritesResults, "locationFavoritesResult", m_rootContext);
    clearList(m_historyResults, "locationHistoryResult", m_rootContext);
}

int NavitQuickProxy::orientation()
{
    return m_settings.get<Tags::Orientation>();
}

void NavitQuickProxy::setOrientation(int orientation)
{
    aDebug() << "Setting orientation to " << orientation;
    try {
        nxeInstance->ipc()->setOrientation(orientation);
        m_settings.set<Tags::Orientation>(orientation);
        aTrace() << "New orientation is " << m_settings.get<Tags::Orientation>();
        emit orientationChanged();

        QTimer::singleShot(500, [this]() {
            nxeInstance->ipc()->render();
        });
    }
    catch (const std::exception& ex) {
        aError() << "An error happened during setting orientation. Ex= " << ex.what();
    }
}

QString NavitQuickProxy::version() const
{
    return QString::fromStdString(gNXEVersion);
}

bool NavitQuickProxy::enablePoi() const
{
    return m_settings.get<Tags::EnablePoi>();
}

void NavitQuickProxy::resize(const QRect& rect)
{
    nxeInstance->resize(rect.width(), rect.height());
}

void NavitQuickProxy::setEnablePoi(bool enable)
{
    aDebug() << (enable ? "Enabling" : "Disabling") << " POI";
    nxeInstance->ipc()->setScheme(enable ? "Car-JLR" : "Car-JLR-nopoi");
    m_settings.set<Tags::EnablePoi>(enable);
}

bool NavitQuickProxy::ftu() const
{
    return m_settings.get<Tags::Ftu>();
}

void NavitQuickProxy::setFtu(bool value)
{
    m_settings.set<Tags::Ftu>(value);

    if (!value) {
        // ftu change to true
        initNavit();
    }

    emit ftuChanged();
}

QObject* NavitQuickProxy::waypointItem() const
{
    return m_waypointItem.data();
}

QObject* NavitQuickProxy::currentlySelectedItem() const
{
    return m_currentItem.data();
}

void NavitQuickProxy::render()
{
    nxeInstance->ipc()->render();
}

void NavitQuickProxy::reset()
{
    aInfo() << "Resetting all data";
    clearList(m_countriesSearchResults, "countrySearchResult", m_rootContext);
    clearList(m_citiesSearchResults, "citySearchResult", m_rootContext);
    clearList(m_streetsSearchResults, "streetSearchResult", m_rootContext);
    clearList(m_addressSearchResults, "addressSearchResult", m_rootContext);
    clearList(m_favoritesResults, "locationFavoritesResult", m_rootContext);
    clearList(m_historyResults, "locationHistoryResult", m_rootContext);


    m_settings.remove();
    nxeInstance->mapDownloader()->removeAllMaps();
    mapsProxy.requestMapsReload();

    setFtu(true);
}

void NavitQuickProxy::quit()
{
    aInfo() << "Quiting application";
    nxeInstance->ipc()->quit();

    emit quitSignal();
}

void NavitQuickProxy::restartNavit()
{
    aInfo() << "Restart Navit";
    nxeInstance->restartNavit();
    QTimer::singleShot(500, this, SLOT(synchronizeNavit()));
}

void NavitQuickProxy::stopNavit()
{
    nxeInstance->stopNavit();
}

void NavitQuickProxy::zoomToRoute()
{
    nxeInstance->ipc()->zoomToRoute();
}

QString NavitQuickProxy::valueFor(const QString& optionName)
{
    QString ret;
    if (optionName == "enablePoi") {
        bool bRet = m_settings.get<Tags::EnablePoi>();
        aDebug() << "value for poi is " << bRet;
        ret = QString("%1").arg(bRet ? "on" : "off");
    }
    else if (optionName == "voice") {
        ret = m_settings.get<Tags::Voice>() ? "on" : "off";
    }
    else if (optionName == "perspective") {
        const std::string value = m_settings.get<Tags::MapView>();
        aTrace() << "Value for pers = " << value;
        return QString::fromStdString(value);
    }

    return ret;
}

void NavitQuickProxy::changeValueFor(const QString& optionName, const QVariant& newVal)
{
    if (optionName == "enablePoi") {
        setEnablePoi(newVal.toString() == "on");
    }
    else if (optionName == "voice") {
        m_settings.set<Tags::Voice>(newVal.toString() == "on");
        nxeInstance->setAudioMute(!(m_settings.get<Tags::Voice>()));
    }
    else if (optionName == "perspective") {
        aTrace() << "Perspective new val = " << newVal.toString().toStdString();
        m_settings.set<Tags::MapView>(newVal.toString().toStdString());
        nxeInstance->ipc()->setPitch(m_settings.get<Tags::MapView>() == "2D" ? 0 : 30);
    }
}

void NavitQuickProxy::startSearch()
{
    finishSearch();
    nxeInstance->ipc()->startSearch();
}

void NavitQuickProxy::finishSearch()
{
    clearList(m_countriesSearchResults, "countrySearchResult", m_rootContext);
    clearList(m_citiesSearchResults, "citySearchResult", m_rootContext);
    clearList(m_streetsSearchResults, "streetSearchResult", m_rootContext);
    clearList(m_addressSearchResults, "addressSearchResult", m_rootContext);
    nxeInstance->ipc()->finishSearch();
}

void NavitQuickProxy::searchCountry(const QString& countryName)
{
    clearList(m_countriesSearchResults, "countrySearchResult", m_rootContext);
    aDebug() << "Search for country = " << countryName.toStdString();
    nxeInstance->ipc()->search(NXE::INavitIPC::SearchType::Country, countryName.toStdString());
}

void NavitQuickProxy::searchCity(const QString& name)
{
    clearList(m_citiesSearchResults, "citySearchResult", m_rootContext);
    aDebug() << "Search for city = " << name.toStdString();
    nxeInstance->ipc()->search(NXE::INavitIPC::SearchType::City, name.toStdString());
}

void NavitQuickProxy::searchStreet(const QString& street)
{
    clearList(m_streetsSearchResults, "streetSearchResult", m_rootContext);
    aDebug() << "Search for street = " << street.toStdString();
    nxeInstance->ipc()->search(NXE::INavitIPC::SearchType::Street, street.toStdString());
}

void NavitQuickProxy::searchAddress(const QString& street)
{
    clearList(m_addressSearchResults, "addressSearchResult", m_rootContext);
    aDebug() << "Search for address = " << street.toStdString();
    nxeInstance->ipc()->search(NXE::INavitIPC::SearchType::Address, street.toStdString());
}

void NavitQuickProxy::searchSelect(const QString& what, int id)
{
    if (id == -1) {
        aInfo() << "Id is -1, so no select";
        return;
    }

    aInfo() << "Selecting id= " << id;
    NXE::INavitIPC::SearchType type;
    if (what == "country") {
        type = NXE::INavitIPC::SearchType::Country;
    }
    else if (what == "city") {
        type = NXE::INavitIPC::SearchType::City;
    }
    else if (what == "street") {
        type = NXE::INavitIPC::SearchType::Street;
    }
    else if (what == "address") {
        type = NXE::INavitIPC::SearchType::Address;
    }
    else {
        throw std::runtime_error("Shouldn't happen");
    }
    nxeInstance->ipc()->selectSearchResult(type, id);
}

void NavitQuickProxy::moveToCurrentPosition()
{
    nxeInstance->ipc()->setTracking(true);
    auto pos = nxeInstance->gps()->position();
    nxeInstance->ipc()->setPosition(pos.longitude, pos.latitude);
}

void NavitQuickProxy::getFavorites()
{
    clearList(m_favoritesResults, "locationFavoritesResult", m_rootContext);

    auto favs = m_settings.favorites();
    aTrace() << "favorites size=" << favs.size();
    std::for_each(favs.begin(), favs.end(), [this](LocationProxy* p) {
        m_favoritesResults.append(p);
    });

    m_rootContext->setContextProperty("locationFavoritesResult", QVariant::fromValue(m_favoritesResults));
}
void NavitQuickProxy::getHistory()
{

    // now we need to get all eta and distances

    m_rootContext->setContextProperty("locationHistoryResult", QVariant::fromValue(m_historyResults));
    emit gettingHistoryDone();
}

void NavitQuickProxy::setZoom(int newZoom)
{
    nxeInstance->ipc()->setZoom(newZoom);
}

void NavitQuickProxy::zoomBy(int factor)
{
    nxeInstance->ipc()->zoomBy(factor);
}

void NavitQuickProxy::clearWaypoint()
{
    setWaypointItem(nullptr);
}

void NavitQuickProxy::setLocationPopUp(const QUuid& id)
{
    aDebug() << Q_FUNC_INFO;

    // clear all popup
    changeCurrentItem(nullptr);

    QObjectList tmp;
    tmp.append(m_citiesSearchResults);
    tmp.append(m_streetsSearchResults);
    tmp.append(m_addressSearchResults);
    tmp.append(m_favoritesResults);
    tmp.append(m_historyResults);

    int newZoomLevel = -1;
    QObject* foundItem = nullptr;
    std::for_each(tmp.begin(), tmp.end(), [this, &id, &foundItem](QObject* o) {
        LocationProxy* proxy = qobject_cast<LocationProxy*>(o);

        if (proxy->id() == id) {
            // we have to copy this, since in a second the model will be deleted
            // and this will points to an deleted object
            foundItem = proxy;
            aDebug() << "Found item " << proxy->itemText().toStdString() << " with id= " << proxy->id().toString().toStdString();
            if (navigationProxy.navigation()) {
                setWaypointItem(proxy);
            } else {
                changeCurrentItem(LocationProxy::clone(proxy));
            }
        }
    });

    if (!m_currentItem) {
        aFatal() << "Unable to find item= " << id.toByteArray().data();
        return;
    }

    // disable tracking

    nxeInstance->ipc()->setTracking(false);

    aInfo() << "Setting location to " << m_currentItem->longitude() << " " << m_currentItem->latitude();
    nxeInstance->ipc()->setPosition(m_currentItem->longitude(), m_currentItem->latitude());
    if (m_citiesSearchResults.contains(foundItem)) {
        newZoomLevel = 4096;
    }
    else if (m_streetsSearchResults.contains(foundItem))
        newZoomLevel = 16;

    else if (m_addressSearchResults.contains(foundItem))
        newZoomLevel = 8;

    aDebug() << " new zoom level = " << newZoomLevel;
    if (newZoomLevel != -1) {
        QTimer::singleShot(100, [this, newZoomLevel]() {
                nxeInstance->ipc()->setZoom(newZoomLevel);
        });
    }
}

void NavitQuickProxy::initNavit()
{
    context->dbusController.start();

    m_rootContext->setContextProperty("countrySearchResult", QVariant::fromValue(m_countriesSearchResults));
    m_rootContext->setContextProperty("citySearchResult", QVariant::fromValue(m_citiesSearchResults));
    m_rootContext->setContextProperty("streetSearchResult", QVariant::fromValue(m_streetsSearchResults));
    m_rootContext->setContextProperty("addressSearchResult", QVariant::fromValue(m_addressSearchResults));
    m_rootContext->setContextProperty("locationFavoritesResult", QVariant::fromValue(m_favoritesResults));
    if (m_settings.get<Tags::Ftu>()) {
        aInfo() << "Ftu is set up no navit need to be run";
        return;
    }
    aInfo() << "Launching navit";

    nxeInstance->startNavit();
    QTimer::singleShot(500, this, SLOT(synchronizeNavit()));
}

void NavitQuickProxy::synchronizeNavit()
{
    // TODO: Synchronize all NavIt settings
    aInfo() << "Synchronizing navit";
    if (m_settings.get<Tags::Ftu>()) {
        return;
    }
    // special case
    nxeInstance->resize(0, 0);

    // set scheme
    setEnablePoi(m_settings.get<Tags::EnablePoi>());
    setOrientation(m_settings.get<Tags::Orientation>());

    // audio
    nxeInstance->setAudioMute(!(m_settings.get<Tags::Voice>()));
    nxeInstance->ipc()->setPitch(m_settings.get<Tags::MapView>() == "2D" ? 0 : 30);
    nxeInstance->ipc()->setTracking(true);
}

void NavitQuickProxy::reloadQueueSlot(const QString& listName, const QObjectList& list)
{
    m_rootContext->setContextProperty(listName, QVariant::fromValue(list));
}

void NavitQuickProxy::changeCurrentItem(LocationProxy* proxy)
{
    nxeInstance->ipc()->clearMapMarker();
    aDebug() << "Currently selected item ptr=" << static_cast<void*>(proxy);
    if (proxy == nullptr ) {
        if (m_currentItem) {
            aTrace() << "Request for clear all current item";
            QObject::disconnect(m_currentItem.data());
            m_currentItem.reset();

            // clear marker
        }
    }
    else {

        // check if this is already in favorites, and if so, change the falue of favorite
        LocationProxy* tmpProxy = proxy;

        auto favIter = std::find_if(m_favoritesResults.begin(), m_favoritesResults.end(), [&proxy, &tmpProxy](QObject* o) -> bool {
            LocationProxy* _proxy = qobject_cast<LocationProxy*>(o);
            if (_proxy->itemText() == proxy->itemText()) {
                aDebug() << "Found item in favorites, use this one ";
                // favorite list is managed by m_favoritesResults and this list will be cleared (and deleted)
                // every time someone calls getFavorites
                tmpProxy = LocationProxy::clone(_proxy);
                return true;
            }

            return false;
        });

        if (favIter != m_favoritesResults.end()) {
            // already in favorites, use favorite item which is not pointed by tmpProxy
            aDebug() << "Since we found a fav we need to free ptr= " << static_cast<void*>(proxy);
            delete proxy;
        }
        aDebug() << "Changing to " << tmpProxy->itemText().toStdString() << " ptr-" << static_cast<void*>(tmpProxy);
        assert(tmpProxy);
        m_currentItem.reset(tmpProxy);
        tmpProxy->moveToThread(this->thread());

        connect(m_currentItem.data(), &LocationProxy::favoriteChanged, [this]() {
                aInfo() << "Adding " << m_currentItem->id().toByteArray().data() << " to favs";
                if (m_currentItem->favorite()) {
                    m_settings.addToFavorites(m_currentItem.data());
                }
                else {
                    m_settings.removeFromFavorites(m_currentItem->id().toByteArray().data());
                }
        });
    }

    if (m_currentItem) {
        // add marker
        aTrace() << "Adding map marker";
        nxeInstance->ipc()->addMapMarker(m_currentItem->longitude(), m_currentItem->latitude());
        // request for distance and eta to this location
        auto pos = nxeInstance->gps()->position();
        NXE::Position pp {m_currentItem->longitude(), m_currentItem->latitude()};
        if (std::isnan(pos.latitude) || std::isnan(pos.longitude)) {
            aError() << "Unable to calculate position";
        } else {
            nxeInstance->ipc()->possibleTrackInformation(pos, pp);
        }
    }


    emit currentlySelectedItemChanged();
}

void NavitQuickProxy::setWaypointItem(LocationProxy *proxy)
{
    nxeInstance->ipc()->setTracking(false);
    if (proxy) {
        auto p = LocationProxy::clone(proxy);
        p->moveToThread(m_rootContext->thread());
        m_waypointItem.reset(p);
        tryToAddToHistory(p);
        nxeInstance->ipc()->addMapMarker(m_waypointItem->longitude(), m_waypointItem->latitude());
    } else {
        m_waypointItem.reset();
        nxeInstance->ipc()->clearMapMarker();
    }

    emit waypointItemChanged();
}

void NavitQuickProxy::tryToAddToHistory(LocationProxy *proxy)
{
    if (proxy) {
        LocationProxy* currItem = proxy;
        auto find = std::find_if(m_historyResults.begin(), m_historyResults.end(), [&currItem](QObject* o) -> bool {
            LocationProxy* p = qobject_cast<LocationProxy*>(o);
            return (p->itemText() == currItem->itemText()) ||
                   (p->id() == currItem->id()) ||
                   (p->longitude() == currItem->longitude() && p->latitude() == currItem->latitude());
        });

        if (find == m_historyResults.end()) {
            aDebug() << "History result wasn't found in history list, adding " << currItem->itemText().toStdString();
            auto p = LocationProxy::clone(currItem);
            p->moveToThread(m_rootContext->thread());
            m_historyResults.append(p);
        } else {
            aInfo() << "Item " << currItem->itemText().toStdString() << " was found in history, skip it";
        }
    }
}
