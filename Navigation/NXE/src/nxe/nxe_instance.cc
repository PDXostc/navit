#include "nxe_instance.h"
#include "inavitprocess.h"
#include "navitcontroller.h"
#include "jsonmessage.h"
#include "settings.h"
#include "settingtags.h"
#include "log.h"
#include "calls.h"
#include "inavitipc.h"

#include <boost/algorithm/string.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string.hpp>

#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <fruit/injector.h>

namespace bipc = boost::interprocess;

namespace NXE {

struct NXEInstancePrivate {

    NXEInstancePrivate(DI::Injector& ifaces, NXEInstance* qptr)
        : navitProcess(ifaces.get<std::shared_ptr<INavitProcess> >())
        , q(qptr)
        , controller(ifaces)
    {
        nTrace() << "NXEInstancePrivate::NXEInstancePrivate()";
    }

    std::shared_ptr<INavitProcess> navitProcess;
    NXEInstance* q;
    NavitController controller;
    Settings settings;
    std::vector<NXEInstance::MessageCbJSON_type> callbacksJSon;

    // Which messages cause a screen redraw
    bool initialized{ false };

    // This is for all messages but 'render'
    void postMessage(const JSONMessage& message)
    {
        nTrace() << "Posting response for " << message.call;

        // This is our internal post message
        try {
            std::for_each(callbacksJSon.begin(), callbacksJSon.end(), [&message](const NXEInstance::MessageCbJSON_type& callback) {
                callback(message);
            });
        }
        catch (const std::exception& ex) {
            nInfo() << "An exception happen when calling a callback of message. We really don't care about it";
        }
    }

    void navitMsgCallback(const JSONMessage& response)
    {
        // At first try to calculate processign time
        postMessage(response);
        nTrace() << "Finished posting response";
    }
};

NXEInstance::NXEInstance(DI::Injector& impls)
    : d(new NXEInstancePrivate{ impls, this })
{
    nDebug() << "Creating NXE instance. Settings path = " << d->settings.configPath();
    nTrace() << "Connecting to navitprocess signals";
    auto bound = std::bind(&NXEInstancePrivate::navitMsgCallback, d.get(), std::placeholders::_1);
    d->controller.addListener(bound);
}

NXEInstance::~NXEInstance()
{
    nTrace() << "~NXEInstance";
    using SettingsTags::Navit::ExternalNavit;
    const bool external = d->settings.get<ExternalNavit>();

    nTrace() << "Stopping controller. external navit=" << external;

    if (!external) {
        d->navitProcess->stop();
    }
}

void NXEInstance::Initialize()
{
    if (d->initialized) {
        nDebug() << "Already initilized";
        return;
    }
    using SettingsTags::Navit::AutoStart;
    using SettingsTags::Navit::ExternalNavit;

    nDebug() << "Initializing NXEInstance";
    bool bAutoRun = d->settings.get<AutoStart>();
    if (bAutoRun) {
        bool external = d->settings.get<ExternalNavit>();
        if (!external) {
            d->navitProcess->start();
        }
        else {
            nInfo() << "Navit external is set, won't run";
        }
        d->controller.tryStart();
    }
    d->initialized = true;
}

void NXEInstance::registerMessageCallback(const NXEInstance::MessageCbJSON_type &cb)
{
    d->callbacksJSon.push_back(cb);
}

bool NXEInstance::HandleMessage(const JSONMessage &msg)
{
    try {
        d->controller.handleMessage(msg);
        return true;
    }
    catch (const std::exception& ex) {
        nFatal() << "Unable to handle message " << msg.call << ", error=" << ex.what();
        NXE::JSONMessage error{ msg.id, msg.call, ex.what() };
        d->postMessage(error);
    }
    return false;
}

void NXEInstance::setWaylandSocketName(const std::string &socketName)
{
    d->navitProcess->setSocketName(socketName);
}

} // namespace NXE
