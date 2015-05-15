#include "navitprocessimpl.h"
#include "log.h"
#include "settings.h"
#include "settingtags.h"

#include <boost/process.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>

#include <boost/program_options/environment_iterator.hpp>

#include <iostream>
#include <set>

namespace bp = boost::process;
namespace bs = boost::system;
namespace bf = boost::filesystem;

namespace {
constexpr pid_t invalidPid = -1;
}
namespace NXE {

struct NavitProcessImplPrivate {

    const std::string m_navitProgramName = "navit";
    std::string socketName = "";
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

bool NavitProcessImpl::start()
{
    std::set<std::string> newEnv;
    Settings settings;
    const std::string commandPath = settings.get<SettingsTags::Navit::Path>();
    const std::string command = commandPath + std::string{ "/" } + d->m_navitProgramName;
    boost::iostreams::file_descriptor_sink sinkOut("/tmp/navit.out");
    boost::iostreams::file_descriptor_sink sinkErr("/tmp/navit.err");

    d->m_lastError = bs::error_code();
    nDebug() << "Starting navit process from " << command << " in dir " << commandPath << " with args ";
    bf::path exe = command;

    try {
        const std::string platform = settings.get<SettingsTags::Navit::GraphicPlatform>();

        int i = 1;
        char* s = *environ;

        for (; s; i++) {
            newEnv.insert(s);
            s = *(environ + i);
        }

        const std::string waylandSocket = "WAYLAND_DISPLAY=" + d->socketName;
        newEnv.insert(waylandSocket);
        if (!platform.empty()) {
            const std::string qt_qpa = "QT_QPA_PLATFORM=" + platform;
            newEnv.insert(qt_qpa);
        }

        d->m_child = bp::execute(bp::initializers::run_exe(exe),
            bp::initializers::start_in_dir(commandPath),
            bp::initializers::set_env(newEnv),
            bp::initializers::throw_on_error(),
            bp::initializers::bind_stdout(sinkOut),
            bp::initializers::bind_stderr(sinkErr));
    }
    catch (const boost::system::system_error& ex) {
        nFatal() << "An exception occured while launching navit from " << exe.string();
        throw ex;
    }

    if (!d->m_lastError) {
        nDebug() << "Navit process properly started";
        d->m_monitor = std::move(std::thread([this]() {
            try {
                nTrace() << "Start monitoring thread";
                int exitVal = bp::wait_for_exit(d->m_child);
                nDebug() << "Navit process exited with " << exitVal;
                d->m_child.pid = invalidPid;
            } catch (const std::exception &ex) {
                nError() << "Exception during waiting, probably because stop() call " << ex.what();
                d->m_child.pid = invalidPid;
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
    }
    if (d->m_monitor.joinable()) {
        d->m_monitor.join();
    }
    d->m_child.pid = invalidPid;
    nDebug() << "Navit stopped";
}

bool NavitProcessImpl::isRunning()
{
    return !(d->m_lastError) && d->m_child.pid != invalidPid;
}

void NavitProcessImpl::setSocketName(const std::string& socket)
{
    d->socketName = socket;
}

} // namespace NXE
