#include "navitquickproxy.h"
#include "jsonmessage.h"
#include "nxe_instance.h"
#include "alog.h"

#include <functional>

extern NXE::NXEInstance* g_nxeInstance;

NavitQuickProxy::NavitQuickProxy(QObject *parent)
    : QObject(parent),
      m_orientation(0)
{
    g_nxeInstance->registerMessageCallback(std::bind(&NavitQuickProxy::navitCallback,
                                                     this, std::placeholders::_1));
}

NavitQuickProxy::~NavitQuickProxy()
{
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
    g_nxeInstance->HandleMessage(msg);
}

void NavitQuickProxy::zoomIn()
{
    boost::property_tree::ptree p;
    p.put("factor", 2);
    NXE::JSONMessage msg{ 1, "zoomBy", "", p };
    g_nxeInstance->HandleMessage(msg);
}

void NavitQuickProxy::zoomOut()
{
    boost::property_tree::ptree p;
    p.put("factor", -2);
    NXE::JSONMessage msg{ 1, "zoomBy", "", p };
    g_nxeInstance->HandleMessage(msg);

}

void NavitQuickProxy::moveTo(int x, int y)
{
    boost::property_tree::ptree p;
    p.put("x", x);
    p.put("y", y);
    NXE::JSONMessage msg{ 1, "moveBy", "", p };
    g_nxeInstance->HandleMessage(msg);
}

void NavitQuickProxy::navitCallback(const NXE::JSONMessage& msg)
{
    aTrace() << "Callback in app";
    // navitCallback
    if ( msg.call == "setOrientation") {
        if (msg.error.empty()) {
            m_orientation = msg.data.get<int>("orientation");
            aTrace() << "New orientation is " << m_orientation;
            emit orientationChanged();
        }
    }
}

