#include "navitcontroller.h"
#include "log.h"

#include <functional>
#include <map>
#include <typeindex>
#include <thread>
#include <chrono>

#include <boost/lexical_cast.hpp>

namespace NXE {

const std::uint16_t timeout = 2;
typedef std::function<bool(NavitController* ctrl, const std::string& data)> CallbackFunction;

bool nonEmptyParser(NavitController* ctrl, const std::string& data, const CallbackFunction& fn)
{
    if (data.empty()) {
        return false;
    }

    return fn(ctrl, data);
}

auto zoomByParser = std::bind(nonEmptyParser, std::placeholders::_1, std::placeholders::_2,
                              [](NavitController* ctrl, const std::string& data) -> bool {
        // data not empty here
        ctrl->zoomBy(boost::lexical_cast<int>(data));
        return true;
    });

struct NavitControllerPrivate {
    const std::map<CallType, CallbackFunction> callbacks{
        { CallType::zoomBy, zoomByParser }
    };

    std::thread m_retriggerThread;
    bool m_isRunning = false;
};

NavitController::NavitController()
    : d_ptr(new NavitControllerPrivate)
{
}

NavitController::~NavitController()
{
}

void NavitController::tryStart()
{
    nDebug() << "Trying to start IPC Navit controller";
    if (!start()) {
        nInfo() << "Unable to start IPC yet, will try to do that in " << timeout << " seconds";
        d_ptr->m_retriggerThread = std::thread([this]() {
            std::chrono::milliseconds dura(timeout * 1000);
            std::this_thread::sleep_for(dura);
            if(!start()) {
                nFatal() << "Unable to start IPC, we won't be able to continue";
            } else {
                d_ptr->m_isRunning = true;
            }
        });
    } else {
        d_ptr->m_isRunning = true;
    }
}

void NavitController::handleMessage(JSONMessage msg)
{
    auto funIter = d_ptr->callbacks.at(msg.call);
    if(!funIter(this, msg.data.get_value_or(""))) {
        nInfo() << "Unable to parser msg ";
    }
}

} // namespace NXE
