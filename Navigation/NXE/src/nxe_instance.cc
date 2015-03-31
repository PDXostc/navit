#include "nxe_instance.h"
#include "navitprocess.h"
#include "navitcontroller.h"
#include "jsonmessage.h"
#include "settings.h"
#include "settingtags.h"
#include "log.h"
#include "calls.h"
#include "navitipc.h"

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

namespace {
const std::string sharedMemoryName{ "Navit_shm" };
const std::uint32_t sharedMemorySize = 7171272;
}

namespace NXE {

struct NXEInstancePrivate {

    NXEInstancePrivate(std::shared_ptr<NavitProcess> p, std::shared_ptr<NavitIPCInterface> w, NXEInstance* qptr)
        : navitProcess(p)
        , q(qptr)
        , controller(w)
    {
    }
    std::shared_ptr<NavitProcess> navitProcess;
    NXEInstance* q;
    NavitController controller;
    Settings settings;
    std::vector<NXEInstance::MessageCb_type> callbacks;
    std::map<std::string, std::chrono::time_point<std::chrono::high_resolution_clock> > timers;
    bipc::shared_memory_object shMem{ bipc::open_or_create, sharedMemoryName.c_str(), bipc::read_write };
    bipc::mapped_region region;
    std::vector<double> perfMeasurement;
    bool initialized {false};

    void postMessage(const JSONMessage& message)
    {
        const std::string rsp = JSONUtils::serialize(message);
        // This is xwalk posting mechanism
        q->PostMessage(rsp.c_str());

        // This is our internal post message
        try {
            std::for_each(callbacks.begin(), callbacks.end(), [&rsp](const NXEInstance::MessageCb_type& callback) {
                callback(rsp);
            });
        } catch(const std::exception &ex) {
            nInfo() << "An exception happen when calling a callback of message. We really don't care about it";
        }

    }

    void navitMsgCallback(const JSONMessage& response)
    {
        if (response.call == "render") {
            nInfo() << "Rendering finished!" << "size=" << sharedMemorySize;
            // read shared memory
            const char* mem = static_cast<const char*>(region.get_address());
            assert(mem);

            q->PostMessage(mem);

            // This is our internal post message
            try {
                std::for_each(callbacks.begin(), callbacks.end(), [&mem](const NXEInstance::MessageCb_type& callback) {
                    callback(std::string {mem, sharedMemorySize});
                });
            } catch(const std::exception &ex) {
                nInfo() << "An exception happen when calling a callback of message. We really don't care about it";
            }
        }
        else {
            postMessage(response);
        }

        auto it = timers.find(response.call);
        if (it != timers.end()) {
            auto now = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> diff = now - timers[response.call];
            double res = diff.count();
            perfMeasurement.push_back(res);
            perfLog(response.call) << " parsing took " << res << " ms";
            timers.erase(it);
        }
    }
};

NXEInstance::NXEInstance(DepInInterfaces &impls)
    : d(new NXEInstancePrivate{ impls.get<std::shared_ptr<NavitProcess>>(),
                                impls.get<std::shared_ptr<NavitIPCInterface>>(), this })
{
    using SettingsTags::Navit::Path;

    nDebug() << "Setting path is" << d->settings.configPath();

    nDebug() << "Creating NXE instance";
    std::string path{ d->settings.get<Path>() };
    nInfo() << "Setting navit path = [" << path << "]";
    d->navitProcess->setProgramPath(path);

    nDebug() << "Connecting to navitprocess signals";
    auto bound = std::bind(&NXEInstancePrivate::navitMsgCallback, d.get(), std::placeholders::_1);
    d->controller.addListener(bound);
}

NXEInstance::~NXEInstance()
{
    nTrace() << "~NXEInstance";
    using SettingsTags::Navit::ExternalNavit;
    const bool external = d->settings.get<ExternalNavit>();

    nTrace() << "Stopping controller. external navit=" << external;

    d->navitProcess->stop();

    if (!external) {
        bipc::shared_memory_object::remove(sharedMemoryName.c_str());
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
    d->shMem.truncate(sharedMemorySize);
    d->region = bipc::mapped_region(d->shMem, bipc::read_only);
    if (bAutoRun) {
        bool external = d->settings.get<ExternalNavit>();
        if (!external) {

            // before running navit, kill
            // all possible running instances
            system("killall navit");

            nInfo() << "Autorun is set, starting Navit";
            d->navitProcess->start();
        } else {
            nInfo() << "Navit external is set, won't run";
        }
        d->controller.tryStart();
    }
    d->initialized = true;
}

void NXEInstance::HandleMessage(const char* msg)
{
    // lock shared ptr
    std::string message{ msg };

    nTrace() << "Message = " << msg;
    boost::algorithm::erase_all(message, " ");
    boost::algorithm::erase_all(message, "\n");
    boost::algorithm::erase_all(message, "\t");

    nDebug() << "Handling message " << message;

    // Eat all exceptions!
    try {
        NXE::JSONMessage jsonMsg = JSONUtils::deserialize(message);

        try {
            d->timers[jsonMsg.call] = std::chrono::high_resolution_clock::now();
            d->controller.handleMessage(jsonMsg);
        }
        catch (const std::exception& ex) {
            nFatal() << "Unable to handle message " << jsonMsg.call << ", error=" << ex.what();
            NXE::JSONMessage error{ jsonMsg.id, jsonMsg.call, ex.what() };
            auto it = d->timers.find(jsonMsg.call);
            if (it != d->timers.end()) {
                d->timers.erase(it);
            }
            d->postMessage(error);
        }
    }
    catch (const std::exception& ex) {
        NXE::JSONMessage error{ 0, "", std::string(ex.what() )};
        nFatal() << "Unable to deserialize message =" << msg;
        nFatal() <<", posting error= " << ex.what();
        d->postMessage(error);
    }
}

void NXEInstance::registerMessageCallback(const NXEInstance::MessageCb_type& cb)
{
    nTrace() << "registering cb";
    d->callbacks.push_back(cb);
}

std::vector<double> NXEInstance::renderMeasurements() const
{
    return d->perfMeasurement;
}

} // namespace NXE
