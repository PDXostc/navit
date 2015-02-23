#include "navitdbus.h"
#include "log.h"

#include <thread>
#include <dbus-c++/dbus.h>

namespace {
const std::string navitDBusDestination = "org.navit_project.navit";
const std::string navitDBusPath = "/org/navit_project/navit/default_navit";
const std::string navitDBusInterface = "org.navit_project.navit.navit";
}

namespace NXE {

struct NavitDBusObjectProxy : public DBus::InterfaceProxy, public DBus::ObjectProxy
{
    NavitDBusObjectProxy(DBus::Connection &con) :
        DBus::InterfaceProxy(navitDBusDestination)
      , DBus::ObjectProxy(con, navitDBusPath, navitDBusInterface.c_str())
    {}
};

struct NavitDBusPrivate {

#if 0
    DBus::Dispatcher::pointer m_dispatcher;
    DBus::Connection::pointer m_connection;
    std::shared_ptr<DBus::ObjectProxy> m_proxy;
#endif

//    io_service io;
//    dbus::endpoint navit_client{ navitDBusDestination, navitDBusPath, navitDBusInterface };
//    dbus::connection conn{ io, dbus::bus::session };

    std::shared_ptr<DBus::Connection> con;
    DBus::BusDispatcher dispatcher;

    bool m_threadRunning = false;
    std::thread m_thread;
};

NavitDBus::NavitDBus()
    : d_ptr(new NavitDBusPrivate)
{
}

NavitDBus::~NavitDBus()
{
    stop();
    if (d_ptr->m_thread.joinable()) {
        d_ptr->m_thread.join();
    }
}

bool NavitDBus::start() noexcept
{
    DBus::default_dispatcher = &d_ptr->dispatcher;
    d_ptr->con.reset(new DBus::Connection { DBus::Connection::SessionBus() } );
    NavitDBusPrivate* d = d_ptr.get();
        d_ptr->m_thread = std::move(std::thread([d]() {
//        d->io.run();
#if 0
    //        DBus::init(true);
    //        d->m_dispatcher = DBus::Dispatcher::create();
    //        d->m_connection =  d->m_dispatcher->create_connection(DBus::BUS_SESSION);
    //        d->m_proxy = d->m_connection->create_object_proxy(navitDBusDestination, navitDBusPath);
    //        nDebug() << d->m_connection->introspect(navitDBusDestination, navitDBusPath);
    //        nDebug() << "Connection " << d->m_connection->is_valid() << d->m_proxy->interfaces().size();
    //        for(auto &&iface: d->m_proxy->interfaces()) {
    //            nDebug() << iface.first;
    //        };

    //        d->m_threadRunning = true;

    //        // now start dispatching! ;)
    //        while(d->m_threadRunning) {
    //            sleep(1);
    //        }
#endif
        }));
    return true;
}

void NavitDBus::stop() noexcept
{
    nDebug() << "Stopping Navit DBus client";
    d_ptr->m_threadRunning = false;
//    d_ptr->io.stop();
#if 0
    d->m_dispatcher->stop();
#endif
}

void NavitDBus::moveBy(double x, double y) noexcept
{
}

void NavitDBus::zoomBy(int y) noexcept
{
    nDebug() << "Zooming by " << y;
#if 0
    if (!d || !d->m_connection) {
        return;
    }
    DBus::ObjectProxy* proxy = d->m_proxy.get();
    DBus::Message::pointer msg = proxy->create_call_message(navitDBusInterface, "zoom");
    msg << y;
    DBus::PendingCall::pointer pending = d->m_connection->send_with_reply_async(msg);
    pending->block();
    DBus::Message::pointer retmsg;

    retmsg = pending->steal_reply();

    if (retmsg->type() == DBus::ERROR_MESSAGE) {
        DBus::ErrorMessage::pointer errormsg = DBus::ErrorMessage::create(retmsg);
        std::cout << "Oops... got an error: " << errormsg->name() << std::endl;
    }
#endif
//    dbus::message m = dbus::message::new_call(d_ptr->navit_client, "zoom");
//    m.pack<int>(y);

//    dbus::message reply = d_ptr->conn.send(m);
//    DBus::CallMessage call;
//    call.member("zoom");
//    DBus::MessageIter it = call.writer();
//    it << y;
//    d_ptr->con->send_blocking(call);
}

} // namespace NXE
