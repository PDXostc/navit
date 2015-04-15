#include "nxe_extension.h"
#include "nxe_instance.h"
#include "inavitipc.h"

#include "navitprocessimpl.h"
#include "navitdbus.h"
#include "gpsdprovider.h"
#include "log.h"
#include "settings.h"
#include "settingtags.h"

#include <fruit/fruit.h>

using namespace NXE;

extern const char kAscii_nxe_api[];
NXE::NXExtension* g_extension = nullptr;

struct NXE::NXExtensionPrivate {
    NXEInstance* instance = nullptr;
    DI::Injector injector{ []() -> DI::Components {
        return fruit::createComponent()
                .bind<INavitIPC, NavitDBus>()
                .bind<INavitProcess, NavitProcessImpl>()
                .bind<IGPSProvider, GPSDProvider>();
    }() };
};

common::Extension* CreateExtension()
{
    NXE::NXExtension::createLogger();
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
    nTrace() << "~NXExtension";
}

common::Instance* NXExtension::CreateInstance()
{
    if (d->instance) {
        nFatal() << "This plugin does not support more than one instance yet";
        return nullptr;
    }

    d->instance = new NXEInstance{ d->injector };

    nDebug() << "Created instance. Ptr= " << static_cast<void*>(d->instance);
    return d->instance;
}

void NXExtension::createLogger()
{
    Settings s;
    const std::string path = s.get<SettingsTags::FileLog>();
    const std::string level = s.get<SettingsTags::LogLevel>();
    std::cout << "logger path= " << path << " level = " << level;
    std::shared_ptr<spdlog::sinks::sink> rot{ new spdlog::sinks::simple_file_sink_mt(path, true) };
    std::shared_ptr<spdlog::sinks::sink> out{ new spdlog::sinks::stdout_sink_mt() };
    spdlog::create("nxe", { rot, out});

    if (level == "debug") {
        spdlog::set_level(spdlog::level::debug);
    }
    else if (level == "warn") {
        spdlog::set_level(spdlog::level::warn);
    }
    else if (level == "trace") {
        spdlog::set_level(spdlog::level::trace);
    }
    else {
        spdlog::set_level(spdlog::level::err);
    }

}
