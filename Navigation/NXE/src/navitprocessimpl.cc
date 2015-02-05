#include "navitprocessimpl.h"

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
    d_ptr(new NavitProcessImplPrivate())
{
}

NavitProcessImpl::~NavitProcessImpl()
{
    if (d_ptr->m_child.pid != invalidPid) {
        // ask via DBus to close itself
        bp::terminate(d_ptr->m_child);
    }
}

void NavitProcessImpl::setProgramPath(const std::string &name)
{
    d_ptr->m_programPath = name;
}

bool NavitProcessImpl::start()
{
    d_ptr->m_lastError = bs::error_code();
    const std::string command =  d_ptr->m_programPath + "/" + d_ptr->m_navitProgramName;
    bf::path exe = command;

    d_ptr->m_child = bp::execute(bp::initializers::run_exe(exe),
                                 bp::initializers::start_in_dir(d_ptr->m_programPath),
                                 bp::initializers::inherit_env(),
                                 bp::initializers::set_on_error(d_ptr->m_lastError));

    return !d_ptr->m_lastError;
}

void NavitProcessImpl::stop()
{
}

bool NavitProcessImpl::isRunning()
{
    return !(d_ptr->m_lastError);
}

void NavitProcessImpl::setArgs(const std::list<std::string> &args)
{
    d_ptr->m_args = args;
}

} // namespace NXE

