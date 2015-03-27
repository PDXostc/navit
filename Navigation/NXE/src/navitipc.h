#ifndef NAVITIPC_H
#define NAVITIPC_H

#include <functional>
#include <boost/signals2/signal.hpp>

namespace NXE {

class NavitIPCInterface {
public:
    typedef boost::signals2::signal<void (std::string)> SpeechSignal;
    typedef boost::signals2::signal<void ()> InitializedSignal;

    virtual ~NavitIPCInterface() {}

    //! IPC start/stop
    virtual void start() = 0;
    virtual void stop(bool quit) = 0;

    // Actuall interface description
    virtual void moveBy(int x, int y) = 0;
    virtual int zoom() = 0;
    virtual void zoomBy(int factor) = 0;
    virtual void render() = 0;

    virtual int orientation() = 0;
    virtual void setOrientation(int newOrientation) = 0;

    // Signals from IPC
    virtual SpeechSignal& speechSignal() = 0;
    virtual InitializedSignal& initializedSignal() = 0;
};

} // NXE

#endif // NAVITIPC_H

