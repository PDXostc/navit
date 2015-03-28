#ifndef NXE_NAVITCONTROLLER_H
#define NXE_NAVITCONTROLLER_H

#include <memory>
#include <functional>
#include "jsonmessage.h"

namespace NXE {

struct NavitControllerPrivate;
class NavitIPCInterface;
class NavitController
{
public:
    typedef std::function<void (const JSONMessage &) > Callback_type;

    // basic ctor
    NavitController(std::shared_ptr<NavitIPCInterface> ipc );
    virtual ~NavitController();

    // LBS functions
    virtual void positon();

    //! Common functions
    void tryStart() ;
    void handleMessage(const JSONMessage &msg);
    void addListener(const Callback_type &cb);

private:
    std::unique_ptr<NavitControllerPrivate> d;
};

} // namespace NXE

#endif // NXE_NAVITCONTROLLER_H
