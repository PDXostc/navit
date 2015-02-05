#ifndef NXE_NAVITPROCESSIMPL_H
#define NXE_NAVITPROCESSIMPL_H

#include "navitprocess.h"
#include <memory>

namespace NXE {

struct NavitProcessImplPrivate;
class NavitProcessImpl : public NavitProcess
{
public:
    NavitProcessImpl();
    ~NavitProcessImpl();

    virtual void setProgramPath(const std::string &name) override final;
    virtual bool start() override final;
    virtual void stop() override final;
    virtual bool isRunning() override final;
    virtual void setArgs(const std::list<std::string> &args) override final;
private:
    std::unique_ptr<NavitProcessImplPrivate> d_ptr;
};

} // namespace NXE

#endif // NXE_NAVITPROCESSIMPL_H
