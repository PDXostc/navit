#include "navitquickproxy.h"
#include "jsonmessage.h"
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
    NXE::INavitIPC * ipc { new NXE::NavitDBus{dbusController}};
    NXE::IMapDownloader * md { new NXE::MapDownloaderDBus{dbusController}};
    NXE::ISpeech* speech { new NXE::SpeechImplDBus{dbusController}};
    NXE::DI::Injector injector{ [this]() -> NXE::DI::Components {
        return fruit::createComponent()
                .bindInstance(*ipc)
                .bind<NXE::INavitProcess, NXE::NavitProcessImpl>()
                .bind<NXE::IGPSProvider, NXE::GPSDProvider>()
                .bindInstance(*md)
                .bindInstance(*speech);
    }() };
};

NavitQuickProxy::NavitQuickProxy(const QString &socketName, QObject* parent)
    : QObject(parent)
    , context(new Context)
    , nxeInstance(new NXE::NXEInstance{ context->injector })
{
    nxeInstance->setWaylandSocketName(socketName.toLatin1().data());
    nxeInstance->Initialize();
    nxeInstance->registerMessageCallback([this](const NXE::JSONMessage& msg) {
        aTrace() << "Callback in app";
        // navitCallback
        if (msg.call == "setOrientation") {
            if (msg.error.empty()) {
                int orientation = msg.data.get<int>("orientation");
                if (orientation == -1 ) {
                    m_settings.set<Tags::Orientation>("Heads-up");
                } else {
                    m_settings.set<Tags::Orientation>("North-up");
                }
                aTrace() << "New orientation is " << m_settings.get<Tags::Orientation>();
                emit orientationChanged();
            }
        } else if(msg.call== "position") {
            aDebug() << "Received position update";
            double lat = msg.data.get<double>("latitude");
            double lon = msg.data.get<double>("longitude");
            double alt = msg.data.get<double>("altitude");
            m_position = QString("%1 %2 %3").arg(lat).arg(lon).arg(alt);
            emit positionChanged();
        } else if(msg.call == "setScheme") {
            const std::string schemeName = msg.data.get<std::string>("scheme");
            if (schemeName == "Car-JLR") {
                m_settings.set<Tags::EnablePoi>(true);
            } else {
                m_settings.set<Tags::EnablePoi>(false);
            }
        }
    });
}

int NavitQuickProxy::orientation()
{
    return m_settings.get<Tags::Orientation>() == "North-up"? 0 : -1;
}

void NavitQuickProxy::setOrientation(int orientation)
{
    boost::property_tree::ptree p;
    p.put("orientation", orientation);
    NXE::JSONMessage msg{ 1, "setOrientation", "", p };
    nxeInstance->HandleMessage(msg);
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
    boost::property_tree::ptree p;
    p.put("scheme", enable ? "Car-JLR" : "Car-JLR-nopoi");
    nxeInstance->HandleMessage(NXE::JSONMessage {1,"setScheme", "", p });
}

void NavitQuickProxy::zoomIn()
{
    boost::property_tree::ptree p;
    p.put("factor", 2);
    NXE::JSONMessage msg{ 1, "zoomBy", "", p };
    nxeInstance->HandleMessage(msg);
}

void NavitQuickProxy::zoomOut()
{
    boost::property_tree::ptree p;
    p.put("factor", -2);
    NXE::JSONMessage msg{ 1, "zoomBy", "", p };
    nxeInstance->HandleMessage(msg);
}

void NavitQuickProxy::moveTo(int x, int y)
{
    boost::property_tree::ptree p;
    p.put("x", x);
    p.put("y", y);
    NXE::JSONMessage msg{ 1, "moveBy", "", p };
    nxeInstance->HandleMessage(msg);
}

void NavitQuickProxy::render()
{
    nxeInstance->HandleMessage(NXE::JSONMessage{0, "render"});
}
