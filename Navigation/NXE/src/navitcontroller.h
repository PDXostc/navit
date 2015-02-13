#ifndef NXE_NAVITCONTROLLER_H
#define NXE_NAVITCONTROLLER_H

#include <memory>
#include <functional>
#include "jsonmessage.h"

namespace NXE {

struct NavitControllerPrivate;
class NavitController
{
public:
    NavitController();
    virtual ~NavitController();

    virtual void start() noexcept = 0;
    virtual void stop() noexcept = 0;

    virtual void moveBy(double x, double y) noexcept = 0;

    void handleMessage(JSONMessage msg);

private:
    std::unique_ptr<NavitControllerPrivate> d_ptr;
};

} // namespace NXE

#endif // NXE_NAVITCONTROLLER_H
