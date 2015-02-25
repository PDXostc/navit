#include "nxe_instance.h"
#include "navitprocess.h"
#include "navitcontroller.h"
#include "jsonmessage.h"
#include "settings.h"
#include "settingtags.h"
#include "log.h"
#include "calls.h"

#include <boost/algorithm/string.hpp>

namespace NXE {

struct NXEInstancePrivate {
    std::weak_ptr<NavitProcess> navitProcess;
    std::weak_ptr<NavitController> controller;
    Settings settings;
    std::vector<NXEInstance::MessageCb_type> callbacks;

    void postMessage(NXEInstance* inst, const std::string& message)
    {
        inst->PostMessage(message.c_str());

        std::for_each(callbacks.begin(), callbacks.end(), [&message](const NXEInstance::MessageCb_type& callback) {
            callback(message);
        });
    }
};

NXEInstance::NXEInstance(std::weak_ptr<NavitProcess> process, std::weak_ptr<NavitController> controller)
    : d(new NXEInstancePrivate{ process, controller })
{
    using SettingsTags::Navit::Path;
    using SettingsTags::Navit::AutoStart;

    auto navi = d->navitProcess.lock();
    assert(navi);

    nDebug() << "Creating NXE instance";
    if (navi) {
        std::string path{ d->settings.get<Path>() };
        nInfo() << "Setting navit path = [" << path << "]";
        navi->setProgramPath(path);
    }

    bool bAutoRun = d->settings.get<AutoStart>();
    if (bAutoRun) {
        nInfo() << "Autorun is set, starting Navit";
        navi->start();
    }
}

NXEInstance::~NXEInstance()
{
    auto navit = d->navitProcess.lock();
    if (navit) {
        navit->stop();
    }
}

void NXEInstance::HandleMessage(const char* msg)
{
    // lock shared ptr
    const auto naviProcess = d->navitProcess.lock();
    std::string message{ msg };

    boost::algorithm::erase_all(message, " ");
    boost::algorithm::erase_all(message, "\n");
    boost::algorithm::erase_all(message, "\t");

    nDebug() << "Handling message " << msg;

    if (!naviProcess->isRunning()) {
        if (!naviProcess->start()) {
            d->postMessage(this,"error");
        }
    }

    try {
        auto navit = d->controller.lock();
        assert(navit);
        navit->tryStart();
        navit->handleMessage(JSONUtils::deserialize(message));
    }
    catch (const std::exception& ex) {
        nFatal() << "Unable to parse message, posting error= " << ex.what();
        d->postMessage(this, ex.what());
    }
}

void NXEInstance::registerMessageCallback(const NXEInstance::MessageCb_type& cb)
{
    d->callbacks.push_back(cb);
}

} // namespace NXE
