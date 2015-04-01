#ifndef NXE_NAVITDBUS_H
#define NXE_NAVITDBUS_H

#include "navitcontroller.h"
#include "inavitipc.h"

#include <fruit/macro.h>

namespace NXE {

class NavitDBusPrivate;
class NavitDBus : public INavitIPC {
public:
    INJECT(NavitDBus());
    ~NavitDBus();

    //! This will block until startup signal is received
    virtual void start() override;
    virtual void stop() override;
    virtual void quit() override;
    virtual void moveBy(int x, int y) override;
    virtual void zoomBy(int y) override;
    virtual int zoom() override;
    virtual void render() override;
    virtual int orientation() override;
    virtual void setOrientation(int newOrientation) override;
    virtual void setCenter(double longitude, double latitude) override;

    virtual SpeechSignal& speechSignal() override;
    virtual InitializedSignal& initializedSignal() override;

private:
    std::unique_ptr<NavitDBusPrivate> d;
};

} // namespace NXE

#endif // NXE_NAVITDBUS_H
