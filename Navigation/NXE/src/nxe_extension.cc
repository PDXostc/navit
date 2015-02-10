#include "nxe_extension.h"
#include "nxe_instance.h"

#include "navitprocessimpl.h"
#include "log.h"

using namespace NXE;

extern const char kAscii_nxe_api[];

common::Extension* CreateExtension()
{
    nDebug() << "Creating NXE extension";
    return new NXExtension();
}

NXExtension::NXExtension()
{
    SetExtensionName("nxe");
    SetJavaScriptAPI(kAscii_nxe_api);
}

NXExtension::~NXExtension()
{
}

common::Instance *NXExtension::CreateInstance()
{
    if (!m_navitProcess) {
        // TODO: Properly set Navit path and
        // environment for Navit

        // Use proper DI here?
        m_navitProcess.reset(new NavitProcessImpl);
    }

    return new NXEInstance(m_navitProcess);
}

void NXExtension::OnShutdown(XW_Extension xw_extension)
{
}

void NXExtension::OnInstanceCreated(XW_Instance xw_instance)
{
}

void NXExtension::OnInstanceDestroyed(XW_Instance xw_instance)
{
}
