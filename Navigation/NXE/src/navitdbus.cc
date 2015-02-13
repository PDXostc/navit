#include "navitdbus.h"
#include "log.h"

#include <dbus/connection.hpp>

#include <boost/asio.hpp>
#include <thread>

namespace ba = boost::asio;

namespace {
const std::string navitDBusDestination = "org.navit_project.navit";
const std::string navitDBusInterface = "org.navit_project.navit";
const std::string navitDBusPath = "/org/navit_project/navit/default_navit";
}

namespace NXE {

struct NavitDBusPrivate {

    NavitDBusPrivate()
        : m_navitDBusEndpoint(dbus::endpoint(navitDBusDestination,
                                             navitDBusInterface,
                                             navitDBusPath))
        , m_sessionBus(m_dbusEventLoop, dbus::bus::session)
    {
    }
    NavitDBusPrivate(NavitDBusPrivate const&) = delete;

    ba::io_service m_dbusEventLoop;
    dbus::endpoint m_navitDBusEndpoint;
    dbus::connection m_sessionBus;

    std::thread m_ioThread;
};

NavitDBus::NavitDBus()
    : d_ptr(new NavitDBusPrivate)
{
}

NavitDBus::~NavitDBus()
{
    stop();
    d_ptr->m_ioThread.join();
}

void NavitDBus::start() noexcept
{
    d_ptr->m_ioThread = std::thread([this]() {
        LOG("Staring thread");
        d_ptr->m_dbusEventLoop.run();
        LOG("DBus event loop finished");
    });
}

void NavitDBus::stop() noexcept
{
    LOG("Stopping Navit DBus client");
    d_ptr->m_dbusEventLoop.stop();
}

void NavitDBus::moveBy(double x, double y) noexcept
{
}

} // namespace NXE
