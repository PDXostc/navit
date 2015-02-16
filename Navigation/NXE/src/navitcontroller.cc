#include "navitcontroller.h"
#include "log.h"

#include <functional>
#include <map>
#include <typeindex>
#include <thread>
#include <chrono>

#include <boost/mpl/map.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/fusion/container/map.hpp>

namespace NXE {

const std::uint16_t timeout = 2;

struct Parser {
    ~Parser() {}
    virtual void parse(const std::string &data) = 0;
};

struct MoveByParser : public Parser {
    virtual void parse(const std::string &data) override {
        nDebug() << "Parsing msg with data = " << data;

        if(data.empty()) {
            nFatal() << "Unable to parse moveBy, data is empty";
            throw std::runtime_error("Unable to parse moveBy message, lack of data");
        }
    }
};

struct NavitControllerPrivate {
    const std::map<CallType, std::shared_ptr<Parser>> calls {
        { CallType::moveBy, std::shared_ptr<Parser> { new MoveByParser() } }
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
        nInfo() << "Unable to start IPC yet, will try to do that in " << timeout<< " seconds";
        d_ptr->m_retriggerThread = std::thread( [this](){
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
    d_ptr->calls.at(msg.call)->parse(msg.data.get_value_or(""));
}

} // namespace NXE
