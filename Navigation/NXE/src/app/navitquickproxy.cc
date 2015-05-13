#include "navitquickproxy.h"
#include "nxe_instance.h"
#include "alog.h"
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
{
    nxeInstance->setWaylandSocketName(socketName.toLatin1().data());

    nxeInstance->pointClickedSignal().connect([this](const NXE::PointClicked& pc) {

        QString name, description;

        // We may have more than one entry on click (town, street and some poi)
        // The order of checking is (the least important is checked sooner)
        // 1.) street
        // 2.) poi
        // 3.) town

        // is this a street ?
        auto streetIter = std::find_if(pc.items.begin(), pc.items.end(), [](const std::pair<std::string, std::string>& p) -> bool {
            return boost::algorithm::starts_with(p.first, "street_");
        });
        if (streetIter != pc.items.end()) {
            name = QString::fromStdString(streetIter->second);
        }


        // is this a poi
        auto poiIter = std::find_if(pc.items.begin(), pc.items.end(), [](const std::pair<std::string, std::string>&p) ->bool {
            return boost::algorithm::starts_with(p.first, "poi_");
        });
        if(poiIter != pc.items.end()) {
            aDebug() << "we found a POI " << poiIter->second;
            name = QString::fromStdString(poiIter->second);
        }

        // is this a town
        auto townIter = std::find_if(pc.items.begin(), pc.items.end(), [](const std::pair<std::string, std::string>&p) ->bool {
            return boost::algorithm::starts_with(p.first, "town_label_");
        });
        if(townIter != pc.items.end()) {
            aDebug() << "we found a town " << townIter->second;
            name = QString::fromStdString(townIter->second);
        }

        // if name is still empty and we have only one entry
        if (name.isEmpty() && pc.items.size() == 1) {
            name = QString::fromStdString(pc.items.front().second);
        }

        aDebug() << "Name = " << name.toStdString() << " position = " << pc.position.longitude << " " << pc.position.latitude;
        auto loc = new LocationProxy { name, false, "1234 N Main, Portland, OR 97208", false};
        // TODO: How to translate position?
//        loc->setPosition(std::make_pair(pc.position.longitude, pc.position.latitude));
        // move to parent thread
        loc->setPosition(pc.position);
        loc->moveToThread(this->thread());
        connect(loc, &LocationProxy::favoriteChanged, [this, &loc](){

            if (loc->favorite()){
                m_settings.addToFavorites(loc);
            } else {
                m_settings.removeFromFavorites(loc->id().toByteArray().data());
            }
        });
        emit pointClicked(loc);
    });

    nxeInstance->setPositionUpdateListener([this](const NXE::Position& position) {
        aDebug() << "Received position update";
        double lat = position.latitude;
        double lon = position.longitude;
        double alt = position.altitude;
        m_position = QString("%1 %2 %3").arg(lat).arg(lon).arg(alt);
        emit positionChanged();
    });

    qRegisterMetaType<QObjectList>("QObjectList");
    typedef QQmlListProperty<LocationProxy> LocationProxyList;
    qRegisterMetaType<LocationProxyList>("QQmlListProperty<LocationProxy>");

    QTimer::singleShot(500, this, SLOT(initNavit()));

    m_settings.favorites();
}

int NavitQuickProxy::orientation()
{
    return m_settings.get<Tags::Orientation>();
}

void NavitQuickProxy::setOrientation(int orientation)
{
    aDebug() << "Setting orientation to " << orientation;
    try {
        nxeInstance->HandleMessage<SetOrientationMessageTag>(orientation);
        m_settings.set<Tags::Orientation>(orientation);
        aTrace() << "New orientation is " << m_settings.get<Tags::Orientation>();
        emit orientationChanged();

        QTimer::singleShot(500, [this]() {
            nxeInstance->HandleMessage<RenderMessageTag>();
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

QString NavitQuickProxy::position() const
{
    return m_position;
}

bool NavitQuickProxy::enablePoi() const
{
    return m_settings.get<Tags::EnablePoi>();
}

bool NavitQuickProxy::topBarLocationVisible() const
{
    return m_settings.get<Tags::TopBarLocationVisible>();
}

void NavitQuickProxy::setTopBarLocationVisible(bool value)
{
    m_settings.set<Tags::TopBarLocationVisible>(value);
    topBarLocationVisibleChanged();
}

void NavitQuickProxy::resize(const QRect& rect)
{
    nxeInstance->HandleMessage<ResizeMessageTag>(rect.width(), rect.height());
}

void NavitQuickProxy::setEnablePoi(bool enable)
{
    nxeInstance->HandleMessage<SetSchemeMessageTag>(enable ? "Car-JLR" : "Car-JLR-nopoi");
    m_settings.set<Tags::EnablePoi>(enable);
}

bool NavitQuickProxy::ftu() const
{
    return m_settings.get<Tags::Ftu>();
}

void NavitQuickProxy::setFtu(bool value)
{
    m_settings.set<Tags::Ftu>(value);
}

QObject* NavitQuickProxy::currentlySelectedItem() const
{
    return m_currentItem.data();
}

void NavitQuickProxy::zoomIn()
{
    nxeInstance->HandleMessage<ZoomByMessageTag>(2);
}

void NavitQuickProxy::zoomOut()
{
    nxeInstance->HandleMessage<ZoomByMessageTag>(-2);
}

void NavitQuickProxy::moveTo(int x, int y)
{
    nxeInstance->HandleMessage<MoveByMessageTag>(x, y);
}

void NavitQuickProxy::render()
{
    nxeInstance->HandleMessage<RenderMessageTag>();
}

void NavitQuickProxy::quit()
{
    aInfo() << "Quiting application";
    nxeInstance->HandleMessage<ExitMessageTag>();

    emit quitSignal();
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

    return ret;
}

void NavitQuickProxy::changeValueFor(const QString& optionName, const QVariant& newVal)
{
    if (optionName == "enablePoi") {
        setEnablePoi(newVal.toString() == "on");
    }
    else if (optionName == "voice") {
        m_settings.set<Tags::Voice>(newVal.toString() == "on");
        nxeInstance->HandleMessage<ToggleAudioMessageTag>(m_settings.get<Tags::Voice>());
    }
}

void NavitQuickProxy::startSearch()
{
    finishSearch();
    nxeInstance->HandleMessage<StartSearchTag>();
}

void NavitQuickProxy::finishSearch()
{
    nxeInstance->HandleMessage<FinishSearchTag>();
}

void NavitQuickProxy::searchCountry(const QString& countryName)
{
    clearList(m_countriesSearchResults, "countrySearchResult", m_rootContext);
    aDebug() << "Search for country = " << countryName.toStdString();
    auto countries = nxeInstance->HandleMessage<SearchMessageTag>(NXE::INavitIPC::SearchType::Country, countryName.toStdString());
    for (NXE::SearchResult result : countries) {
        aTrace() << "result " << result.country.name;
        m_countriesSearchResults.append(new LocationProxy{ result });
    }
    aDebug() << "Country Model size = " << m_countriesSearchResults.size() << static_cast<void*>(m_rootContext);
    m_rootContext->setContextProperty("countrySearchResult", QVariant::fromValue(m_countriesSearchResults));

    emit searchDone();
}

void NavitQuickProxy::searchCity(const QString& name)
{
    clearList(m_citiesSearchResults, "citySearchResult", m_rootContext);
    aDebug() << "Search for city = " << name.toStdString();
    auto cities = nxeInstance->HandleMessage<SearchMessageTag>(NXE::INavitIPC::SearchType::City, name.toStdString());
    aDebug() << cities.size();

    for (NXE::SearchResult city : cities) {
        m_citiesSearchResults.append(new LocationProxy{ city });
    }
    aDebug() << "City Model size = " << m_citiesSearchResults.size();
    m_rootContext->setContextProperty("citySearchResult", QVariant::fromValue(m_citiesSearchResults));
    emit searchDone();
}

void NavitQuickProxy::searchStreet(const QString& street)
{
    clearList(m_streetsSearchResults, "streetSearchResult", m_rootContext);

    aDebug() << "Search for street = " << street.toStdString();
    auto streets = nxeInstance->HandleMessage<SearchMessageTag>(NXE::INavitIPC::SearchType::Street, street.toStdString());
    aDebug() << streets.size();

    for (NXE::SearchResult street : streets) {
        m_streetsSearchResults.append(new LocationProxy{ street });
    }
    aDebug() << "Street Model size = " << m_streetsSearchResults.size();
    m_rootContext->setContextProperty("streetSearchResult", QVariant::fromValue(m_streetsSearchResults));
    emit searchDone();
}

void NavitQuickProxy::searchAddress(const QString& street)
{
    clearList(m_addressSearchResults, "addressSearchResult", m_rootContext);

    aDebug() << "Search for address = " << street.toStdString();
    auto results = nxeInstance->HandleMessage<SearchMessageTag>(NXE::INavitIPC::SearchType::Address, street.toStdString());

    for (NXE::SearchResult result : results) {
        m_addressSearchResults.append(new LocationProxy{ result });
    }
    aDebug() << "Address Model size = " << m_addressSearchResults.size();
    m_rootContext->setContextProperty("addressSearchResult", QVariant::fromValue(m_addressSearchResults));
    emit searchDone();
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
    nxeInstance->HandleMessage<SearchSelectMessageTag>(type, id);
}

void NavitQuickProxy::searchNear(const QString& str)
{
    // TODO: Implement me
}

void NavitQuickProxy::getFavorites()
{
    aFatal() << "Not implemented " << __PRETTY_FUNCTION__;

    auto favs = m_settings.favorites();
    std::for_each(favs.begin(), favs.end(), [this] (LocationProxy* p) {
        m_favoritesResults.append(p);
    });

    m_rootContext->setContextProperty("locationFavoritesResult", QVariant::fromValue(m_favoritesResults));

    emit gettingFavoritesDone();
}
void NavitQuickProxy::getHistory()
{
    aFatal() << "Not implemented " << __PRETTY_FUNCTION__;

    m_historyResults.append(new LocationProxy{ "hist_test1", false, "", true });

    m_rootContext->setContextProperty("locationHistoryResult", QVariant::fromValue(m_historyResults));

    emit gettingHistoryDone();
}

void NavitQuickProxy::setTopBarVisibility(bool value)
{
    setTopBarLocationVisible(value);
}

void NavitQuickProxy::setZoom(int newZoom)
{
    nxeInstance->HandleMessage<SetZoomMessageTag>(newZoom);
}

void NavitQuickProxy::setLocationPopUp(const QUuid& id)
{
    aDebug() << Q_FUNC_INFO;
    NXE::Position pos;
    int newZoomLevel = -1;
    std::for_each(m_citiesSearchResults.begin(), m_citiesSearchResults.end(), [this, &id, &pos](QObject* o) {
        LocationProxy* proxy = qobject_cast<LocationProxy*>(o);

        if (proxy->id() == id) {
            // we have to copy this, since in a second the model will be deleted
            // and this will points to an deleted object
            m_currentItem.reset(LocationProxy::clone(proxy));
            pos.latitude = proxy->latitude();
            pos.longitude = proxy->longitude();
        }
    });

    // search in streets
    std::for_each(m_streetsSearchResults.begin(), m_streetsSearchResults.end(), [this, &id, &pos, &newZoomLevel](QObject* o) {
        LocationProxy* proxy = qobject_cast<LocationProxy*>(o);

        if (proxy->id() == id) {
            // we have to copy this, since in a second the model will be deleted
            // and this will points to an deleted object
            m_currentItem.reset(LocationProxy::clone(proxy));
            pos.latitude = proxy->latitude();
            pos.longitude = proxy->longitude();
            newZoomLevel = 16;
        }
    });
    // search in addresses
    std::for_each(m_addressSearchResults.begin(), m_addressSearchResults.end(), [this, &id, &pos, &newZoomLevel](QObject* o) {
        LocationProxy* proxy = qobject_cast<LocationProxy*>(o);

        if (proxy->id() == id) {
            // we have to copy this, since in a second the model will be deleted
            // and this will points to an deleted object
            m_currentItem.reset(LocationProxy::clone(proxy));
            pos.latitude = proxy->latitude();
            pos.longitude = proxy->longitude();
            newZoomLevel = 8;
        }
    });
    // search in favs
    std::for_each(m_favoritesResults.begin(), m_favoritesResults.end(), [this, &id, &pos, &newZoomLevel](QObject* o) {
        LocationProxy* proxy = qobject_cast<LocationProxy*>(o);

        if (proxy->id() == id) {
            // we have to copy this, since in a second the model will be deleted
            // and this will points to an deleted object
            m_currentItem.reset(LocationProxy::clone(proxy));
            pos.latitude = proxy->latitude();
            pos.longitude = proxy->longitude();
        }
    });

    if (!m_currentItem) {
        aFatal() << "Unable to find item= " << id.toByteArray().data();
    }
    else {
        connect(m_currentItem.data(), &LocationProxy::favoriteChanged, [this]() {
            aInfo() << "Adding " << m_currentItem->id().toByteArray().data() << " to favs";
            if (m_currentItem->favorite())
                m_settings.addToFavorites(m_currentItem.data());
            else
                m_settings.removeFromFavorites(m_currentItem->id().toByteArray().data());
        });
        emit currentlySelectedItemChanged();
        aInfo() << "Setting location to " << pos.longitude << " " << pos.latitude;
        nxeInstance->HandleMessage<SetPositionMessageTag>(pos.longitude, pos.latitude);

        // HACK: For some reasons setting zoom directly after
        // sending position message causes a deadlock
        if (newZoomLevel != -1) {
            QTimer::singleShot(100, [this, newZoomLevel]() {
                nxeInstance->HandleMessage<SetZoomMessageTag>(newZoomLevel);
            });
        }
    }
}
void NavitQuickProxy::hideLocationBars()
{
    m_currentItem.reset(nullptr);
    setTopBarLocationVisible(false);
    currentlySelectedItemChanged();
}

void NavitQuickProxy::initNavit()
{
    m_rootContext->setContextProperty("countrySearchResult", QVariant::fromValue(m_countriesSearchResults));
    m_rootContext->setContextProperty("citySearchResult", QVariant::fromValue(m_citiesSearchResults));
    m_rootContext->setContextProperty("streetSearchResult", QVariant::fromValue(m_streetsSearchResults));
    m_rootContext->setContextProperty("addressSearchResult", QVariant::fromValue(m_addressSearchResults));
    aInfo() << "Launching navit";
    context->dbusController.start();

    nxeInstance->Initialize();
    QTimer::singleShot(500, this, SLOT(synchronizeNavit()));
}

void NavitQuickProxy::synchronizeNavit()
{
    // TODO: Synchronize all NavIt settings
    aInfo() << "Synchronizing navit";
    // special case
    nxeInstance->HandleMessage<ResizeMessageTag>(0, 0);

    // set scheme
    setEnablePoi(m_settings.get<Tags::EnablePoi>());
    setOrientation(m_settings.get<Tags::Orientation>());

    // audio
    nxeInstance->HandleMessage<ToggleAudioMessageTag>(m_settings.get<Tags::Voice>());
}
