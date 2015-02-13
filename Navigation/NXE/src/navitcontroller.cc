#include "navitcontroller.h"
#include "log.h"

#include <functional>
#include <map>
#include <typeindex>

#include <boost/mpl/map.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/fusion/container/map.hpp>

namespace NXE {

struct Parser {
    ~Parser() {}
    virtual void parse(const std::string &data) = 0;
};

struct MoveByParser : public Parser {
    virtual void parse(const std::string &data) override {
        nDebug() << "Parsing msg with data = " << data;

        if(data.empty()) {
            throw std::runtime_error("Unable to parse moveBy message, lack of data");
        }
    }
};

struct NavitControllerPrivate {
    const std::map<CallType, std::shared_ptr<Parser>> calls {
        { CallType::moveBy, std::shared_ptr<Parser> { new MoveByParser() } }
    };
};

NavitController::NavitController()
    : d_ptr(new NavitControllerPrivate)
{
}

NavitController::~NavitController()
{
}

void NavitController::handleMessage(JSONMessage msg)
{
    d_ptr->calls.at(msg.call)->parse(msg.data.get_value_or(""));
}

} // namespace NXE
