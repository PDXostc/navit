#include "navitquickproxy.h"
#include "jsonmessage.h"

#include "nxe_instance.h"

extern NXE::NXEInstance* g_nxeInstance;

NavitQuickProxy::NavitQuickProxy(QObject *parent) : QObject(parent)
{
}

NavitQuickProxy::~NavitQuickProxy()
{
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

