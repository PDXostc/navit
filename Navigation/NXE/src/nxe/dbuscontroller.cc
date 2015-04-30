#include "dbuscontroller.h"
#include "log.h"

#include <dbus-c++/dbus.h>
#include <thread>
#include <chrono>

namespace NXE {

struct DBusControllerPrivate {
    std::unique_ptr<DBus::Connection> conn;
    std::thread dispatchingThread;
    bool threadRunning {false};
    DBus::BusDispatcher dispatcher;
};

DBusController::DBusController()
    : d(new DBusControllerPrivate)
{
    nTrace() << __PRETTY_FUNCTION__;
    start();
}

DBus::Connection &DBusController::connection() const noexcept
{
    assert(d->conn);
    return *(d->conn);
}

void DBusController::start()
{
    if (d->threadRunning) {
        nInfo() << "Dispatcher already running";
        return;
    }

    nInfo() << "Starting DBus dispatching";
    ::DBus::default_dispatcher = &d->dispatcher;
    d->dispatchingThread = std::move(std::thread([this]() {
        nTrace() << "Dispatching";
        d->threadRunning = true;
        ::DBus::default_dispatcher->enter();
        nTrace() << "EOF dispatching";
    }));

    std::chrono::milliseconds dura(50);
    while (!d->threadRunning) {
        nTrace() << "Thread not running, waiting";
        std::this_thread::sleep_for(dura);
    }
    d->conn.reset(new DBus::Connection {DBus::Connection::SessionBus()});
    nTrace() << "EOF start";
}

DBusController::~DBusController()
{
    nDebug() << "Stopping DBus Controller";
    if (d->conn) {
        d->conn->disconnect();
    }
    if (::DBus::default_dispatcher == &d->dispatcher) {
        ::DBus::default_dispatcher->leave();
    }

    if (d->dispatchingThread.joinable()) {
        d->dispatchingThread.join();
    }
}

} // namespace NXE

