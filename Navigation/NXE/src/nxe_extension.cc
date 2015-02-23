#include "nxe_extension.h"
#include "nxe_instance.h"

#include "navitprocessimpl.h"
#include "navitdbus.h"
#include "log.h"

using namespace NXE;

extern const char kAscii_nxe_api[];

namespace {
const std::string g_logPath = "/tmp/nxe.log";
}

common::Extension* CreateExtension()
{
    //spdlog::rotating_logger_mt("nxe_logger", g_logPath, 1048576 * 5, 3);
    spdlog::stdout_logger_mt("nxe_logger");
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
