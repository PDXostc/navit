#include "navitdbus.h"
#include <dbus/connection.hpp>

#include <boost/asio.hpp>

namespace ba = boost::asio;

namespace {
const std::string navitDBusDestination = "org.navit_project.navit";
const std::string navitDBusInterface = "org.navit_project.navit";
const std::string navitDBusPath = "/org/navit_project/navit/default_navit";
}

namespace NXE {

struct NavitDBusPrivate {

    NavitDBusPrivate():
        m_navitDBusEndpoint(dbus::endpoint(navitDBusDestination,
                                           navitDBusInterface,
                                           navitDBusPath)),
        m_sessionBus(m_dbusEventLoop, dbus::bus::session)
    {}
    NavitDBusPrivate(NavitDBusPrivate const&) = delete;

    ba::io_service m_dbusEventLoop;
    dbus::endpoint m_navitDBusEndpoint;
    dbus::connection m_sessionBus;
};

NavitDBus::NavitDBus():
    d_ptr(new NavitDBusPrivate)
{
}

NavitDBus::~NavitDBus()
{
}

void NavitDBus::start() noexcept
{

    d_ptr->m_dbusEventLoop.run();
}

void NavitDBus::stop() noexcept
{
    d_ptr->m_dbusEventLoop.stop();
}

void NavitDBus::pan() noexcept
{

}

void NavitDBus::scale() noexcept
{
}

void NavitDBus::downloadMap() noexcept
{
}

void NavitDBus::calculateRoute() noexcept
{

}

} // namespace NXE

