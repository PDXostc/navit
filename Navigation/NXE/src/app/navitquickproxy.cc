#include "navitquickproxy.h"
#include "nxe_instance.h"
#include "alog.h"
#include "fruit/fruit.h"
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

struct Context {
    NXE::DBusController dbusController;
    NXE::INavitIPC* ipc{ new NXE::NavitDBus{ dbusController } };
    NXE::IMapDownloader* md{ new NXE::MapDownloaderDBus{ dbusController } };
#if defined(NXE_OS_LINUX)
    NXE::ISpeech* speech{ nullptr};
#elif defined(NXE_OS_TIZEN)
    NXE::ISpeech* speech{ new NXE::SpeechImplDBus{ dbusController } };
#endif
    NXE::DI::Injector injector{ [this]() -> NXE::DI::Components {
        return fruit::createComponent()
                .bindInstance(*ipc)
                .bind<NXE::INavitProcess, NXE::NavitProcessImpl>()
                .bind<NXE::IGPSProvider, NXE::GPSDProvider>()
                .bindInstance(*md)
                .bindInstance(*speech);
    }() };
};

NavitQuickProxy::NavitQuickProxy(const QString& socketName, QQmlContext* ctx, QObject* parent)
    : QObject(parent)
    , context(new Context)
    , nxeInstance(new NXE::NXEInstance{ context->injector })
    , m_rootContext(ctx)
    , mapsProxy(nxeInstance)
{
    context->dbusController.start();
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

        aDebug() << "Name = " << name.toStdString();
        auto loc = new LocationProxy {name, false, "1234 N Main, Portland, OR 97208", false};
        // move to parent thread
        loc->moveToThread(this->thread());
        emit pointClicked(loc);
    });

    nxeInstance->Initialize();

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

    synchronizeNavit();
}

int NavitQuickProxy::orientation()
{
    return m_settings.get<Tags::Orientation>() == "North-up" ? 0 : -1;
}

void NavitQuickProxy::setOrientation(int orientation)
{
    aDebug() << "Setting orientation to " << orientation;
    try {
        nxeInstance->HandleMessage<SetOrientationMessageTag>(orientation);
        if (orientation == -1) {
            m_settings.set<Tags::Orientation>("Heads-up");
        }
        else {
            m_settings.set<Tags::Orientation>("North-up");
        }
        aTrace() << "New orientation is " << m_settings.get<Tags::Orientation>();
        emit orientationChanged();
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
    return m_currentItem;
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

    return ret;
}

void NavitQuickProxy::changeValueFor(const QString& optionName, const QVariant& newVal)
{
    if (optionName == "enablePoi") {
        setEnablePoi(newVal.toString() == "on");
    }
}

void NavitQuickProxy::startSearch()
{
    nxeInstance->HandleMessage<StartSearchTag>();
}

void NavitQuickProxy::searchCountry(const QString& countryName)
{
    auto countries = nxeInstance->HandleMessage<SearchCountryLocationTag>(countryName.toStdString());
    for (NXE::Country country : countries) {
        m_searchResults.append(new LocationProxy{ QString::fromStdString(country.name),
            false, "", false });
    }
    m_rootContext->setContextProperty("locationSearchResult", QVariant::fromValue(m_searchResults));

    emit searchDone();
}

void NavitQuickProxy::searchCity(const QString& name)
{
}

void NavitQuickProxy::getFavorites()
{
    aFatal() << "Not implemented " << __PRETTY_FUNCTION__;

    m_favoritesResults.append(new LocationProxy{ "fav_test1", false, "", true });

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

void NavitQuickProxy::setLocationPopUp(const QString& name)
{
    aFatal() << "Not implemented " << __PRETTY_FUNCTION__;
    // TODO: This is a fake implementation for now
    if (m_searchResults.size() != 0) {
        m_currentItem = qobject_cast<LocationProxy*>(m_searchResults.at(0));
        currentlySelectedItemChanged();
    }
    else if (m_favoritesResults.size() != 0) {
        m_currentItem = qobject_cast<LocationProxy*>(m_favoritesResults.at(0));
        currentlySelectedItemChanged();
    }
    else if (m_historyResults.size() != 0) {
        m_currentItem = qobject_cast<LocationProxy*>(m_historyResults.at(0));
        currentlySelectedItemChanged();
    }
}
void NavitQuickProxy::hideLocationBars()
{
    m_currentItem = nullptr;
    setTopBarLocationVisible(false);
    currentlySelectedItemChanged();
}

void NavitQuickProxy::synchronizeNavit()
{
    aInfo() << "Synchronizing navit";
    // TODO: Synchronize all NavIt settings

    // set scheme
    setEnablePoi(m_settings.get<Tags::EnablePoi>());
}
