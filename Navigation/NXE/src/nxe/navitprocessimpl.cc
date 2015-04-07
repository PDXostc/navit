#include "navitprocessimpl.h"
#include "log.h"

#include <boost/process.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>

#include <iostream>

namespace bp = boost::process;
namespace bs = boost::system;
namespace bf = boost::filesystem;

namespace {
constexpr pid_t invalidPid = -1;
}
namespace NXE {

struct NavitProcessImplPrivate {

    const std::string m_navitProgramName = "navit";
    std::string m_programPath = "";
    std::list<std::string> m_args = {};
    bp::child m_child = bp::child(invalidPid);
    bs::error_code m_lastError = bs::error_code(2, bs::generic_category());
    std::thread m_monitor;
    bool m_bShouldBeRunning = false;
};

NavitProcessImpl::NavitProcessImpl()
    : d(new NavitProcessImplPrivate())
{
    nTrace() << "NavitProcessImpl::NavitProcessImpl()";
}

NavitProcessImpl::~NavitProcessImpl()
{
    stop();
}

void NavitProcessImpl::setProgramPath(const std::string& name)
{
    d->m_programPath = name;
}

bool NavitProcessImpl::start()
{
    d->m_lastError = bs::error_code();
    const std::string command = d->m_programPath + std::string{ "/" } + d->m_navitProgramName;
    nDebug() << "Starting navit process from " << command << " in dir " << d->m_programPath << " with args ";
    bf::path exe = command;

    boost::iostreams::file_descriptor_sink sinkOut("/tmp/navit.out");
    boost::iostreams::file_descriptor_sink sinkErr("/tmp/navit.err");
    d->m_child = bp::execute(bp::initializers::run_exe(exe),
        bp::initializers::start_in_dir(d->m_programPath),
        bp::initializers::inherit_env(),
        bp::initializers::set_on_error(d->m_lastError),
        bp::initializers::bind_stdout(sinkOut),
        bp::initializers::bind_stderr(sinkErr));

    if (!d->m_lastError) {
        nDebug() << "Navit process properly started";
        d->m_monitor = std::move(std::thread([this]() {
            try {
                nTrace() << "Start monitoring thread";
                int exitVal = bp::wait_for_exit(d->m_child);
                nDebug() << "Navit process exited with " << exitVal;
            } catch (const std::exception &ex) {
                nError() << "Exception during waiting, probably because stop() call";
            }
        }));
    }
    else {
        nError() << d->m_lastError.message();
        d->m_child.pid = invalidPid;
    }

    return !d->m_lastError;
}

void NavitProcessImpl::stop()
{
    nDebug() << "Stopping navit Pid=" << d->m_child.pid;
    if (d->m_child.pid != invalidPid) {
        bp::terminate(d->m_child);
        d->m_monitor.join();
    }
    d->m_child.pid = invalidPid;
}

bool NavitProcessImpl::isRunning()
{
    return !(d->m_lastError) && d->m_child.pid != invalidPid;
}

void NavitProcessImpl::setArgs(const std::list<std::string>& args)
{
    d->m_args = args;
}

} // namespace NXE
