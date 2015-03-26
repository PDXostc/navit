#include "navitdbus.h"
#include "log.h"

#include <thread>
#include <dbus-c++/dbus.h>
#include "dbus_helpers.hpp"

#include <boost/signals2/signal.hpp>

namespace {
const std::string navitDBusDestination = "org.navit_project.navit";
const std::string navitDBusPath = "/org/navit_project/navit/navit/0";
const std::string navitDBusInterface = "org.navit_project.navit.navit";
}

namespace NXE {

struct NavitDBusObjectProxy : public ::DBus::InterfaceProxy, public ::DBus::ObjectProxy {
    NavitDBusObjectProxy(::DBus::Connection& con)
        : ::DBus::InterfaceProxy(navitDBusInterface)
        , ::DBus::ObjectProxy(con, navitDBusPath, navitDBusDestination.c_str())
    {
        nDebug() << "Connect to signal";
        connect_signal(NavitDBusObjectProxy, signal, speechCallback);
        connect_signal(NavitDBusObjectProxy, startup, startupCallback);
    }

    int zoom()
    {
        return NXE::DBus::getAttr<int>("zoom", *this);
    }

    void zoomBy(int factor)
    {
        DBus::call("zoom", *this, factor);
    }

    void stop() {
        DBus::call("quit", *this);
    }

    // Implement a generic DBus method
    void moveBy(int x, int y)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter it = call.writer();
        call.member("set_center_screen");
        ::DBus::Struct<int, int> val;
        val._1 = x;
        val._2 = y;
        it << val;
        ::DBus::Message ret = invoke_method(call);
        if (ret.is_error()) {
            nFatal() << "Unable to call "
                     << "set_center_screen";
            throw std::runtime_error("Unable to call moveBy");
        }
    }

    void render()
    {
        DBus::call("draw", *this);
    }

    void speechCallback(const ::DBus::SignalMessage& sig)
    {
        ::DBus::MessageIter it = sig.reader();
        std::map<std::string, ::DBus::Variant> res;
        it >> res;

        auto val = std::find_if(res.begin(), res.end(), [](const std::pair<std::string, ::DBus::Variant>& val) -> bool {
            const std::string strVal = val.second;
            return val.first == "type" && strVal == "speech";
        });

        if (val != res.end()) {
            std::string data = res["data"];
            speechSignal(data);
        }
    }

    void startupCallback(const ::DBus::SignalMessage&)
    {
        initializedSignal();
    }

    boost::signals2::signal<void(std::string)> speechSignal;
    boost::signals2::signal<void()> initializedSignal;
};

struct NavitDBusPrivate {

    std::shared_ptr< ::DBus::Connection> con;
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
    nTrace() << "Destroying navit dbus";
    stop();
    if (d->m_thread.joinable()) {
        d->m_thread.join();
    }
}

void NavitDBus::start()
{
    nTrace() << "Staring DBus dispatching";
    if (d->m_threadRunning) {
        return;
    }

    ::DBus::default_dispatcher = &d->dispatcher;
    d->con.reset(new ::DBus::Connection{ ::DBus::Connection::SessionBus() });
    d->object.reset(new NavitDBusObjectProxy(*(d->con.get())));

    d->m_thread = std::move(std::thread([this]() {
        d->m_threadRunning = true;
        ::DBus::default_dispatcher->enter();
    }));
}

void NavitDBus::stop()
{
    if (!d->m_threadRunning) {
        // nothing to do really ;)
        return;
    }

    nDebug() << "Gracefully quit Navit process";
    d->object->stop();

    nDebug() << "Stopping Navit DBus client";
    ::DBus::default_dispatcher->leave();
    if (d->con) {
        d->con->disconnect();
    }
    d->m_threadRunning = false;
}

void NavitDBus::moveBy(int x, int y)
{
    d->object->moveBy(x, y);
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

NavitIPCInterface::SpeechSignal& NavitDBus::speechSignal()
{
    assert(d && d->object);
    return d->object->speechSignal;
}

NavitIPCInterface::InitializedSignal& NavitDBus::initializedSignal()
{
    assert(d && d->object);
    return d->object->initializedSignal;
}

} // namespace NXE
