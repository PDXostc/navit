#include "navitprocessimpl.h"
#include "log.h"

#include <boost/process.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>

#include <iostream>

namespace bp = ::boost::process;
namespace bs = ::boost::system;
namespace bf = ::boost::filesystem;

namespace {
constexpr pid_t invalidPid = -1;
}
namespace NXE {

struct NavitProcessImplPrivate {

    const std::string m_navitProgramName = "navit";
    std::string m_programPath            = "";
    std::list<std::string> m_args        = {};
    bp::child m_child                    = bp::child(invalidPid);
    bs::error_code m_lastError           = bs::error_code(2,bs::generic_category());
};

NavitProcessImpl::NavitProcessImpl():
    d(new NavitProcessImplPrivate())
{
}

NavitProcessImpl::~NavitProcessImpl()
{
    nDebug() << __PRETTY_FUNCTION__;
    stop();
}

void NavitProcessImpl::setProgramPath(const std::string &name)
{
    d->m_programPath = name;
}

bool NavitProcessImpl::start()
{
    d->m_lastError = bs::error_code();
    std::string command = d->m_programPath + "/" + d->m_navitProgramName;
    nDebug() << "Starting navit process from " << command << " in dir " << d->m_programPath << " with args ";
    bf::path exe = command;

    boost::iostreams::file_descriptor_sink sink("/tmp/what.out");
    d->m_child = bp::execute(bp::initializers::run_exe(exe),
                             bp::initializers::start_in_dir(d->m_programPath),
                             bp::initializers::inherit_env(),
                             bp::initializers::set_on_error(d->m_lastError),
                             bp::initializers::bind_stdout(sink),
                             bp::initializers::bind_stderr(sink));
    nTrace() << d->m_lastError.message();
    return !d->m_lastError;
}

void NavitProcessImpl::stop()
{
    nDebug() << "Stopping navit";
    if (d->m_child.pid != invalidPid) {
        // ask via DBus to close itself
        nDebug() << "Terminating navit process";
        bp::terminate(d->m_child);
    }
}

bool NavitProcessImpl::isRunning()
{
    return !(d->m_lastError);
}

void NavitProcessImpl::setArgs(const std::list<std::string> &args)
{
    d->m_args = args;
}

} // namespace NXE

