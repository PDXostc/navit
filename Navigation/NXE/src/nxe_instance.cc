#include "nxe_instance.h"
#include "navitprocess.h"

#include "jsonmessage.h"

namespace NXE {

struct NXEInstancePrivate {
    std::weak_ptr<NavitProcess> m_navitProcess;
};

NXEInstance::NXEInstance(std::weak_ptr<NavitProcess> process) :
    d_ptr(new NXEInstancePrivate{ process })
{
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
