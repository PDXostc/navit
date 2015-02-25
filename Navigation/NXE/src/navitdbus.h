#ifndef NXE_NAVITDBUS_H
#define NXE_NAVITDBUS_H

#include "navitcontroller.h"

namespace NXE {

class NavitDBusPrivate;
class NavitDBus : public NavitController {
public:
    NavitDBus();
    ~NavitDBus();

    virtual void start() override;
    virtual void stop() override;
    virtual void moveBy(double x, double y) override;
    virtual void zoomBy(int y) override;
    virtual int zoom() override;

private:
    std::unique_ptr<NavitDBusPrivate> d;
};

} // namespace NXE

#endif // NXE_NAVITDBUS_H
