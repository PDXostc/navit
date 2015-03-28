#include "navitdbus.h"
#include "log.h"

#include <thread>
#include <chrono>
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
        inProgress = true;
        int val = NXE::DBus::getAttr<int>("zoom", *this);
        inProgress = false;
        return val;
    }

    void zoomBy(int factor)
    {
        inProgress = true;
        DBus::call("zoom", *this, factor);
        inProgress = false;
    }

    void quit() {
        inProgress = true;
        DBus::call("quit", *this);
        inProgress = false;
    }

    // Implement a generic DBus method
    void moveBy(int x, int y)
    {
        inProgress = true;
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
        inProgress = false;
    }

    void render()
    {
        inProgress = true;
        DBus::call("draw", *this);
        inProgress = false;
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

    int orientation()
    {
        inProgress = true;
        int orientation = DBus::getAttr<int>("orientation", *this);
        inProgress = false;

        return orientation;
    }

    void setOrientation(int newOrientation)
    {
        nInfo() << "Changing orientation to " << newOrientation;
        ::DBus::Variant val;
        ::DBus::MessageIter ww = val.writer();
        ww << newOrientation;
        DBus::setAttr("orientation", *this, val);
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
        std::chrono::milliseconds dura( 1000 );
        std::this_thread::sleep_for(dura);
    }

    nTrace() << "Navit DBus started";
}

void NavitDBus::stop()
{
    while (d && d->object && d->object->inProgress) {
        nInfo() << "A signal processing is in progress we have to wait";
        std::chrono::milliseconds dura( 30 );
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
    DBus::call("quit",*(d->object.get()));
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

int NavitDBus::orientation()
{
    return d->object->orientation();
}

void NavitDBus::setOrientation(int newOrientation)
{
    if (newOrientation != 0 && newOrientation != -1 ) {
        throw std::runtime_error("Unable to change orientation. Incorrect value, value can only be -1/0");
    }
    d->object->setOrientation(newOrientation);
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
