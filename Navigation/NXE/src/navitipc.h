#ifndef NAVITIPC_H
#define NAVITIPC_H

namespace NXE {

class NavitIPCInterface {
public:
    virtual ~NavitIPCInterface() {}

    //! IPC start/stop
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void moveBy(double x, double y) = 0;
    virtual int zoom() = 0;
    virtual void zoomBy(int factor) = 0;
    virtual void render() = 0;
};

} // NXE

#endif // NAVITIPC_H

