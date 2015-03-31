#ifndef NAVITIPC_H
#define NAVITIPC_H

#include <functional>
#include <boost/signals2/signal.hpp>

namespace NXE {

class INavitIPC {
public:
    typedef boost::signals2::signal<void (std::string)> SpeechSignal;
    typedef boost::signals2::signal<void ()> InitializedSignal;

    virtual ~INavitIPC() {}

    //! IPC start/stop
    virtual void start() = 0;
    // stops monitoring 
    virtual void stop() = 0;

    virtual void quit() = 0;

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

