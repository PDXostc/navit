#ifndef INAVITPROCESS_H
#define INAVITPROCESS_H

#include <list>
#include <string>

namespace NXE {

class INavitProcess {
public:
    virtual ~INavitProcess() {}

    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() = 0;

    virtual void setSocketName(const std::string& socket) = 0;
};

} // namespace NXE

#endif // INAVITPROCESS_H
