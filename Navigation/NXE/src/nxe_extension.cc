#include "nxe_extension.h"
#include "nxe_instance.h"

#include "navitprocessimpl.h"
#include "navitdbus.h"
#include "log.h"
#include "settings.h"
#include "settingtags.h"

using namespace NXE;

extern const char kAscii_nxe_api[];
NXE::NXExtension* g_extension = nullptr;

struct NXE::NXExtensionPrivate {
    std::shared_ptr<NavitProcess> navitProcess;
    std::shared_ptr<NavitController> navitController;

    NXEInstance* instance = nullptr;
};

common::Extension* CreateExtension()
{
    Settings s;
    const std::string path = s.get<SettingsTags::FileLog>();
    spdlog::rotating_logger_mt("nxe_logger", path, 1048576 * 5, 3, true);
    spdlog::set_level(spdlog::level::debug);
    g_extension = new NXExtension();
    nInfo() << "Plugin loaded. Addr" << static_cast<void*>(g_extension);
    return g_extension;
}

NXExtension::NXExtension()
    : d(new NXExtensionPrivate)
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
    if (d->instance) {
        nFatal() << "This plugin does not support more than one instance yet";
        return nullptr;
    }

    if (!d->navitProcess) {

        // TODO: Properly set Navit path and
        // environment for Navit

        // Use proper DI here?
        d->navitProcess.reset(new NavitProcessImpl);
        d->navitController.reset(new NavitDBus);
    }

    d->instance =  new NXEInstance(d->navitProcess, d->navitController);

    nDebug() << "Created instance. Ptr= " << static_cast<void*>(d->instance);
    return d->instance;
}

//void NXExtension::OnShutdown(XW_Extension xw_extension)
//{
//    nDebug() << "On Shutdown " << xw_extension;
//    delete g_extension;
//}

//void NXExtension::OnInstanceCreated(XW_Instance xw_instance)
//{
//    nDebug() << "Instance created " << xw_instance;
//    if(g_extension) {
//        g_extension->d->instance->Initialize();
//    }
//}

//void NXExtension::OnInstanceDestroyed(XW_Instance xw_instance)
//{
//    nDebug() << "Instance destroyed " << xw_instance;
//    if (g_extension) {
//        g_extension->d->instance.reset();
//    }
//}
