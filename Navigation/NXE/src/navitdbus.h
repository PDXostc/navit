#ifndef NXE_NAVITDBUS_H
#define NXE_NAVITDBUS_H

#include "navitcontroller.h"

namespace NXE {

class NavitDBus : public NavitController
{
public:
    ~NavitDBus();
    virtual void pan() noexcept override;
    virtual void scale() noexcept override;
    virtual void downloadMap() noexcept override;

    virtual void calculateRoute() noexcept override;
};

} // namespace NXE

#endif // NXE_NAVITDBUS_H
