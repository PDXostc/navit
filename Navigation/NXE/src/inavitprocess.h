#ifndef INAVITPROCESS_H
#define INAVITPROCESS_H

#include <list>
#include <string>

namespace NXE {

class INavitProcess
{
public:
    virtual ~INavitProcess() {}

    virtual void setProgramPath(const std::string &name) = 0;

    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() = 0;

    virtual void setArgs(const std::list<std::string> &args) = 0;
};

} // namespace NXE

#endif // INAVITPROCESS_H
