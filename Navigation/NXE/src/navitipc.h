#ifndef NAVITIPC_H
#define NAVITIPC_H

#include <functional>

namespace NXE {

class NavitIPCInterface {
public:

    typedef std::function<void (std::string)> SpeechCallback;

    virtual ~NavitIPCInterface() {}

    //! IPC start/stop
    virtual void start() = 0;
    virtual void stop() = 0;

    // Actuall interface description
    virtual void moveBy(int x, int y) = 0;
    virtual int zoom() = 0;
    virtual void zoomBy(int factor) = 0;
    virtual void render() = 0;

    virtual void registerSpeechCallback(const SpeechCallback &cb) = 0;
};

} // NXE

#endif // NAVITIPC_H

