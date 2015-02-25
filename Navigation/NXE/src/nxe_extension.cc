#include "nxe_extension.h"
#include "nxe_instance.h"

#include "navitprocessimpl.h"
#include "navitdbus.h"
#include "log.h"
#include "settings.h"
#include "settingtags.h"

using namespace NXE;

extern const char kAscii_nxe_api[];

common::Extension* CreateExtension()
{
    Settings s;
    const std::string path = s.get<SettingsTags::FileLog>();
    spdlog::rotating_logger_mt("nxe_logger", path, 1048576 * 5, 3);
    spdlog::set_level(spdlog::level::debug);
    nInfo() << "Plugin loaded";
    return new NXExtension();
}

NXExtension::NXExtension()
{
    nDebug() << "Creating NXE extension";
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
        m_navitController.reset(new NavitDBus);
    }

    return new NXEInstance(m_navitProcess, m_navitController);
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
