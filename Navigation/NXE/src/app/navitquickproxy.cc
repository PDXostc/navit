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
    , m_orientation(0)
{
    nxeInstance->setWaylandSocketName(socketName.toLatin1().data());
    nxeInstance->Initialize();
    nxeInstance->registerMessageCallback([this](const NXE::JSONMessage& msg) {
        aTrace() << "Callback in app";
        // navitCallback
        if (msg.call == "setOrientation") {
            if (msg.error.empty()) {
                m_orientation = msg.data.get<int>("orientation");
                aTrace() << "New orientation is " << m_orientation;
                emit orientationChanged();
            }
        } else if(msg.call== "position") {
//            m_position = msg.data.get
            aDebug() << "Received position update";
            double lat = msg.data.get<double>("latitude");
            double lon = msg.data.get<double>("longitude");
            double alt = msg.data.get<double>("altitude");
            m_position = QString("%1 %2 %3").arg(lat).arg(lon).arg(alt);
            emit positionChanged();
        }
    });
}

int NavitQuickProxy::orientation()
{
    return m_orientation;
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
