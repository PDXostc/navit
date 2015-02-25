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

    //! IPC start/stop
    virtual bool start() = 0;
    virtual void stop() = 0;

    //! An IPC interface
    virtual void moveBy(double x, double y) = 0;


    virtual int zoom() = 0;
    virtual void zoomBy(int factor) = 0;

    //! Common functions
    void tryStart() ;
    void handleMessage(JSONMessage msg);

private:
    std::unique_ptr<NavitControllerPrivate> d_ptr;
};

} // namespace NXE

#endif // NXE_NAVITCONTROLLER_H
