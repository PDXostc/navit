#ifndef NXE_NAVITPROCESS_H
#define NXE_NAVITPROCESS_H

#include <list>
#include <string>

namespace NXE {

class NavitProcess
{
public:
    virtual ~NavitProcess() {}

    virtual void setProgramPath(const std::string &name) = 0;

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() = 0;

    virtual void setArgs(const std::list<std::string> &args) = 0;
};

} // namespace NXE

#endif // NXE_NAVITPROCESS_H
