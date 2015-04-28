#ifndef NAVITIPC_H
#define NAVITIPC_H

#include <functional>
#include <boost/signals2/signal.hpp>

namespace NXE {

class INavitIPC {
public:
    typedef boost::signals2::signal<void(std::string)> SpeechSignal;
    typedef boost::signals2::signal<void()> InitializedSignal;

    virtual ~INavitIPC() {}

    virtual void quit() = 0;

    // Actuall interface description
    virtual void moveBy(int x, int y) = 0;
    virtual int zoom() = 0;
    virtual void zoomBy(int factor) = 0;
    virtual void render() = 0;

    virtual int orientation() = 0;
    virtual void setOrientation(int newOrientation) = 0;

    virtual void setCenter(double longitude, double latitude) = 0;
    virtual void setDestination(double longitude, double latitude, const std::string& description) = 0;
    virtual void setPosition(double longitude, double latitude) = 0;
    virtual void clearDestination() = 0;

    virtual void setScheme(const std::string& scheme) = 0;

    // Signals from IPC
    virtual SpeechSignal& speechSignal() = 0;
    virtual InitializedSignal& initializedSignal() = 0;
};

} // NXE

#endif // NAVITIPC_H
