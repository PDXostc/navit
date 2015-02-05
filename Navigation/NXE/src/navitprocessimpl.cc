#include "navitprocessimpl.h"

#include <QtCore/QProcess>

namespace NXE {

struct NavitProcessImplPrivate {
    const std::string m_navitProgramName = "navit";
    std::string m_programPath;

    std::list<std::string> m_args;
    QProcess m_process;
};


NavitProcessImpl::NavitProcessImpl():
    d_ptr(new NavitProcessImplPrivate())
{
}

NavitProcessImpl::~NavitProcessImpl()
{
}

void NavitProcessImpl::setProgramPath(const std::string &name)
{
    d_ptr->m_programPath = name;
}

void NavitProcessImpl::start()
{
    const std::string command =  d_ptr->m_programPath + "/" + d_ptr->m_navitProgramName;
    d_ptr->m_process.setProgram(command.c_str());
    d_ptr->m_process.start();
}

void NavitProcessImpl::stop()
{
}

bool NavitProcessImpl::isRunning()
{
    return true;
}

void NavitProcessImpl::setArgs(const std::list<std::string> &args)
{
    d_ptr->m_args = args;
}

} // namespace NXE

