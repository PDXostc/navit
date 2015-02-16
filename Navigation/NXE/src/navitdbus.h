#ifndef NXE_NAVITDBUS_H
#define NXE_NAVITDBUS_H

#include "navitcontroller.h"

namespace NXE {

class NavitDBusPrivate;
class NavitDBus : public NavitController {
public:
    NavitDBus();
    ~NavitDBus();

    virtual bool start() noexcept override;
    virtual void stop() noexcept override;
    virtual void moveBy(double x, double y) noexcept override;

private:
    std::unique_ptr<NavitDBusPrivate> d_ptr;
};

} // namespace NXE

#endif // NXE_NAVITDBUS_H
