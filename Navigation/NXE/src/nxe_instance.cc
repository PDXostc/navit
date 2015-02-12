#include "nxe_instance.h"
#include "navitprocess.h"
#include "jsonmessage.h"
#include "settings.h"

namespace NXE {

struct NXEInstancePrivate {
    std::weak_ptr<NavitProcess> m_navitProcess;
    Settings m_settings;
};

NXEInstance::NXEInstance(std::weak_ptr<NavitProcess> process) :
    d_ptr(new NXEInstancePrivate{ process })
{
    auto navi = d_ptr->m_navitProcess.lock();

    if (navi) {
        navi->setProgramPath(d_ptr->m_settings.get<std::string>("navit.path"));
    }
}

NXEInstance::~NXEInstance()
{
}

void NXEInstance::HandleMessage(const char *msg)
{
    // lock shared ptr
    const auto navi = d_ptr->m_navitProcess.lock();
    const std::string message {msg};

    if (!navi->isRunning()) {
        if(!navi->start()) {
            // unable to start Navit, we won't work
        }
    }
}

} // namespace NXE
