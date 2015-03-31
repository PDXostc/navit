#ifndef NXE_NAVITPROCESSIMPL_H
#define NXE_NAVITPROCESSIMPL_H

#include "inavitprocess.h"
#include <memory>
#include <fruit/macro.h>

namespace NXE {

struct NavitProcessImplPrivate;
class NavitProcessImpl : public INavitProcess
{
public:
    INJECT(NavitProcessImpl());
    ~NavitProcessImpl();

    virtual void setProgramPath(const std::string &name) override final;
    virtual bool start() override final;

    /*!
    @brief Wait for Navit process finish.
    Navit will finish if a quit message has been sent to it from IPC, if not
    this function will wait forever
    @todo: Change the name as this has nothing to do with stopping
     */
    virtual void stop() override final;
    virtual bool isRunning() override final;
    virtual void setArgs(const std::list<std::string> &args) override final;
private:
    std::unique_ptr<NavitProcessImplPrivate> d;
};

} // namespace NXE

#endif // NXE_NAVITPROCESSIMPL_H
