#ifndef NXE_NAVITCONTROLLER_H
#define NXE_NAVITCONTROLLER_H

#include <memory>

namespace NXE {

struct NavitControllerPrivate;
class NavitController
{
public:
    NavitController();
    virtual ~NavitController();

    virtual void start() noexcept = 0;
    virtual void stop() noexcept = 0;

    virtual void pan() noexcept = 0;
    virtual void scale() noexcept = 0;
    virtual void downloadMap() noexcept = 0;

    virtual void calculateRoute() noexcept = 0;

private:
    std::unique_ptr<NavitControllerPrivate> d_ptr;
};

} // namespace NXE

#endif // NXE_NAVITCONTROLLER_H
