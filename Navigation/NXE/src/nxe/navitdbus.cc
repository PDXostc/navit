#include "navitdbus.h"
#include "log.h"

#include <thread>
#include <chrono>
#include <dbus-c++/dbus.h>
#include "dbus_helpers.hpp"

#include <boost/signals2/signal.hpp>
#include <boost/format.hpp>

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

    void speechCallback(const ::DBus::SignalMessage& sig)
    {
        nDebug() << "Speech callback";
        inProgress = true;
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

        nTrace() << "Speech callback ended";
        inProgress = false;
    }

    void startupCallback(const ::DBus::SignalMessage&)
    {
        inProgress = true;
        nTrace() << "Navit has started";
        initializedSignal();
        inProgress = false;
    }

    boost::signals2::signal<void(std::string)> speechSignal;
    boost::signals2::signal<void()> initializedSignal;
    bool inProgress = false;
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
    nTrace() << "NavitDBus::NavitDBus()";
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
        nTrace() << "Dispatching";
        d->m_threadRunning = true;
        ::DBus::default_dispatcher->enter();
        nTrace() << "EOF dispatching";
    }));

    // wait until dispatching thread is started
    while (!d->m_threadRunning) {
        std::chrono::milliseconds dura(1000);
        std::this_thread::sleep_for(dura);
    }

    nTrace() << "Navit DBus started";
}

void NavitDBus::stop()
{
    while (d && d->object && d->object->inProgress) {
        nInfo() << "A signal processing is in progress we have to wait";
        std::chrono::milliseconds dura(30);
        std::this_thread::sleep_for(dura);
    }

    nTrace() << "Signal processing is done!";

    if (!d->m_threadRunning) {
        // nothing to do really ;)
        nInfo() << "Navit DBus Already stopped";
        return;
    }

    nDebug() << "Stopping Navit DBus client";
    if (d->con) {
        d->con->disconnect();
        nTrace() << "Object disconnected";
    }

    ::DBus::default_dispatcher->leave();
    nTrace() << "Dispatcher leave";

    d->m_threadRunning = false;
    nDebug() << "Done stoping dbus";
}

void NavitDBus::quit()
{
    DBus::call("quit", *(d->object.get()));
}

void NavitDBus::moveBy(int x, int y)
{
    ::DBus::CallMessage call;
    ::DBus::MessageIter it = call.writer();
    call.member("set_center_screen");
    ::DBus::Struct<int, int> val;
    val._1 = x;
    val._2 = y;
    DBus::call("set_center_screen",*(d->object.get()), val);
}

void NavitDBus::zoomBy(int y)
{
    nDebug() << "Zooming by " << y;
    DBus::call("zoom", *(d->object.get()), y);
}

int NavitDBus::zoom()
{
    nDebug() << "Getting zoom";
    return  NXE::DBus::getAttr<int>("zoom", *(d->object.get()));
}

void NavitDBus::render()
{
    nDebug() << "Rendering";
    DBus::call("draw", *(d->object.get()));
    nDebug() << "Rendering finished";
}

int NavitDBus::orientation()
{
    return DBus::getAttr<int>("orientation", *(d->object.get()));
}

void NavitDBus::setOrientation(int newOrientation)
{
    nDebug() << "Changing orientation to " << newOrientation;
    if (newOrientation != 0 && newOrientation != -1) {
        throw std::runtime_error("Unable to change orientation. Incorrect value, value can only be -1/0");
    }
    DBus::setAttr("orientation", *(d->object.get()), newOrientation);
}

void NavitDBus::setCenter(double longitude, double latitude)
{
    auto format = boost::format("geo: %1% %2%") % longitude % latitude;
    const std::string message = format.str();

    DBus::call("set_center_by_string", *(d->object.get()), message);
}

INavitIPC::SpeechSignal& NavitDBus::speechSignal()
{
    assert(d && d->object);
    return d->object->speechSignal;
}

INavitIPC::InitializedSignal& NavitDBus::initializedSignal()
{
    assert(d && d->object);
    return d->object->initializedSignal;
}

} // namespace NXE
