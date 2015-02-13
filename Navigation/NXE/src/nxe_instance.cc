#include "nxe_instance.h"
#include "navitprocess.h"
#include "navitcontroller.h"
#include "jsonmessage.h"
#include "settings.h"
#include "settingtags.h"
#include "log.h"

#include <boost/algorithm/string.hpp>

namespace NXE {

struct NXEInstancePrivate {
    std::weak_ptr<NavitProcess> m_navitProcess;
    std::weak_ptr<NavitController> m_controller;
    Settings m_settings;
};

NXEInstance::NXEInstance(std::weak_ptr<NavitProcess> process, std::weak_ptr<NavitController> controller)
    : d_ptr(new NXEInstancePrivate{ process, controller })
{
    using SettingsTags::Navit::Path;
    using SettingsTags::Navit::AutoStart;

    auto navi = d_ptr->m_navitProcess.lock();

    nDebug() << "Creating NXE instance";
    if (navi) {
        std::string path{ d_ptr->m_settings.get<Path>() };
        nInfo() << "Setting navit path = [" << path << "]";
        navi->setProgramPath(path);
    }

    bool bAutoRun = d_ptr->m_settings.get<AutoStart>();
    if (bAutoRun) {
        nInfo() << "Autorun is set, starting Navit";
        navi->start();
    }
}

NXEInstance::~NXEInstance()
{
}

void NXEInstance::HandleMessage(const char* msg)
{
    // lock shared ptr
    const auto navi = d_ptr->m_navitProcess.lock();
    std::string message{ msg };

    boost::algorithm::erase_all(message, " ");
    boost::algorithm::erase_all(message, "\n");
    boost::algorithm::erase_all(message, "\t");

    nDebug() << "Handling message " << msg;

    if (!navi->isRunning()) {
        if (!navi->start()) {
            // unable to start Navit, we won't work
        }
    }

    auto navit = d_ptr->m_controller.lock();
    navit->handleMessage(JSONUtils::deserialize(message));
}

} // namespace NXE
