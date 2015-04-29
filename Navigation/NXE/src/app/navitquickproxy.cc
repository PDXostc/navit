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

#include <functional>

struct Context {
    NXE::DBusController dbusController;
    NXE::INavitIPC* ipc{ new NXE::NavitDBus{ dbusController } };
    NXE::IMapDownloader* md{ new NXE::MapDownloaderDBus{ dbusController } };
    NXE::ISpeech* speech{ new NXE::SpeechImplDBus{ dbusController } };
    NXE::DI::Injector injector{ [this]() -> NXE::DI::Components {
        return fruit::createComponent()
                .bindInstance(*ipc)
                .bind<NXE::INavitProcess, NXE::NavitProcessImpl>()
                .bind<NXE::IGPSProvider, NXE::GPSDProvider>()
                .bindInstance(*md)
                .bindInstance(*speech);
    }() };
};

NavitQuickProxy::NavitQuickProxy(const QString& socketName, QObject* parent)
    : QObject(parent)
    , context(new Context)
    , nxeInstance(new NXE::NXEInstance{ context->injector })
{
    nxeInstance->setWaylandSocketName(socketName.toLatin1().data());
    nxeInstance->Initialize();
//    nxeInstance->registerMessageCallback([this](const NXE::JSONMessage& msg) {
//        aTrace() << "Callback in app";
//        // navitCallback
//        if (msg.call == "setOrientation") {
//            if (msg.error.empty()) {
//                int orientation = msg.data.get<int>("orientation");
//            }
//        } else if(msg.call == "setScheme") {
//            const std::string schemeName = msg.data.get<std::string>("scheme");
//            if (schemeName == "Car-JLR") {
//                m_settings.set<Tags::EnablePoi>(true);
//            } else {
//                m_settings.set<Tags::EnablePoi>(false);
//            }
//        }
//    });

    // mapDownloaderCallbacks!
    mapDownloaderListener.progressCb = [this](const std::string&, std::uint64_t, std::uint64_t) {
    };
    mapDownloaderListener.errorCb = [this](const std::string&, const std::string&) {
    };
    mapDownloaderListener.finishedCb = [this](const std::string&) {
    };

    nxeInstance->setMapDownloaderListener(mapDownloaderListener);

    nxeInstance->setPositionUpdateListener([this](const NXE::Position& position) {
        aDebug() << "Received position update";
        double lat = position.latitude;
        double lon = position.longitude;
        double alt = position.altitude;
        m_position = QString("%1 %2 %3").arg(lat).arg(lon).arg(alt);
        emit positionChanged();
    });
}

int NavitQuickProxy::orientation()
{
    return m_settings.get<Tags::Orientation>() == "North-up" ? 0 : -1;
}

void NavitQuickProxy::setOrientation(int orientation)
{
    aDebug() << "Setting orientation to " << orientation;
    try {
        nxeInstance->HandleMessage123<SetOrientationMessageTag>(orientation);
        if (orientation == -1 ) {
            m_settings.set<Tags::Orientation>("Heads-up");
        } else {
            m_settings.set<Tags::Orientation>("North-up");
        }
        aTrace() << "New orientation is " << m_settings.get<Tags::Orientation>();
        emit orientationChanged();
    } catch (const std::exception& ex) {
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

void NavitQuickProxy::setEnablePoi(bool enable)
{
    nxeInstance->HandleMessage123<SetSchemeMessageTag>( enable ? "Car-JLR" : "Car-JLR-nopoi");
}

void NavitQuickProxy::zoomIn()
{
    nxeInstance->HandleMessage123<ZoomByMessageTag>(2);
}

void NavitQuickProxy::zoomOut()
{
    nxeInstance->HandleMessage123<ZoomByMessageTag>(-2);
}

void NavitQuickProxy::moveTo(int x, int y)
{
    nxeInstance->HandleMessage123<MoveByMessageTag>(x,y);
}

void NavitQuickProxy::render()
{
    nxeInstance->HandleMessage123<RenderMessageTag>();
    //    nxeInstance->HandleMessage(NXE::JSONMessage{0, "render"});
}
