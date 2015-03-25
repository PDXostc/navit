#include "navitdbus.h"
#include "log.h"

#include <thread>
#include <dbus-c++/dbus.h>
#include "dbus_helpers.hpp"

namespace {
const std::string navitDBusDestination = "org.navit_project.navit";
const std::string navitDBusPath = "/org/navit_project/navit/default_navit";
const std::string navitDBusInterface = "org.navit_project.navit.navit";
}

namespace NXE {

struct NavitDBusObjectProxy : public ::DBus::InterfaceProxy, public ::DBus::ObjectProxy {
    NavitDBusObjectProxy(::DBus::Connection& con)
        : ::DBus::InterfaceProxy(navitDBusInterface)
        , ::DBus::ObjectProxy(con, navitDBusPath, navitDBusDestination.c_str())
    {
    }

    int zoom() {
        return NXE::DBus::getAttr<int>("zoom", *this);
    }

    void zoomBy(int factor) {
        DBus::call("zoom", *this, factor);
    }

    // Implement a generic DBus method
    void moveBy(int x, int y) {
        ::DBus::CallMessage call;
        ::DBus::MessageIter it = call.writer();
        call.member("set_center_screen");
        ::DBus::Struct<int, int> val;
        val._1 = x;
        val._2 = y;
        it << val;
        ::DBus::Message ret = invoke_method(call);
        if (ret.is_error()) {
            nFatal() << "Unable to call " << "set_center_screen";
            throw std::runtime_error("Unable to call moveBy");
        }
    }

    void render() {
        DBus::call("draw",*this);
    }
};

struct NavitDBusPrivate {

    std::shared_ptr<::DBus::Connection> con;
    std::shared_ptr<NavitDBusObjectProxy> object;
    ::DBus::BusDispatcher dispatcher;

    bool m_threadRunning = false;
    std::thread m_thread;
};

NavitDBus::NavitDBus()
    : d(new NavitDBusPrivate)
{
}

NavitDBus::~NavitDBus()
{
    nDebug() << "Destroying navit dbus";
    stop();
    if (d->m_thread.joinable()) {
        d->m_thread.join();
    }
}

void NavitDBus::start()
{
    if (d->m_threadRunning) {
        return;
    }

    ::DBus::default_dispatcher = &d->dispatcher;
    d->con.reset(new ::DBus::Connection{ ::DBus::Connection::SessionBus() });
    d->object.reset(new NavitDBusObjectProxy(*(d->con.get())));

    d->m_thread = std::move(std::thread([this]() {
        while(d->m_threadRunning) {
            nDebug() << "Dispatching";
            d->dispatcher.dispatch();
        }
    }));
}

void NavitDBus::stop()
{
    nDebug() << "Stopping Navit DBus client";
    if (d->con) {
        d->con->disconnect();
    }
    d->m_threadRunning = false;
}

void NavitDBus::moveBy(int x, int y)
{
    d->object->moveBy(x,y);
}

void NavitDBus::zoomBy(int y)
{
    nDebug() << "Zooming by " << y;
    d->object->zoomBy(y);
}

int NavitDBus::zoom()
{
    nDebug() << "Getting zoom";
    return d->object->zoom();
}

void NavitDBus::render()
{
    nDebug() << "Rendering";
    return d->object->render();
}

} // namespace NXE
