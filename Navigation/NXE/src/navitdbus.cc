#include "navitdbus.h"
#include "log.h"

#include <thread>
#include <dbus-c++/dbus.h>

namespace {
const std::string navitDBusDestination = "org.navit_project.navit";
const std::string navitDBusPath = "/org/navit_project/navit/default_navit";
const std::string navitDBusInterface = "org.navit_project.navit.navit";

template<typename R>
R getAttr(const std::string &attrName, DBus::InterfaceProxy &proxy) {
    nDebug() << "Getting attribute = " << attrName;
    DBus::CallMessage call;
    DBus::MessageIter it = call.writer();
    call.member("get_attr");
    it << attrName;
    DBus::Message ret = proxy.invoke_method(call);
    if (ret.is_error()) {
        throw std::runtime_error("Unable to call zoom");
    }
    nDebug() << "Unpacking";
    std::string attr;
    DBus::MessageIter retIter = ret.reader();
    retIter >> attr ;
    DBus::Variant v;
    retIter >> v;
    R value = static_cast<R>(v);
    nDebug() << attrName << "= " << value;

    return value;
}

template<typename A>
void call(const std::string &methodName, DBus::InterfaceProxy &proxy, A attr)
{
    DBus::CallMessage call;
    DBus::MessageIter it = call.writer();
    call.member(methodName.c_str());
    it << attr;
    DBus::Message ret = proxy.invoke_method(call);
    if (ret.is_error()) {
        throw std::runtime_error("Unable to call zoom");
    }
}

}

namespace NXE {

struct NavitDBusObjectProxy : public DBus::InterfaceProxy, public DBus::ObjectProxy {
    NavitDBusObjectProxy(DBus::Connection& con)
        : DBus::InterfaceProxy(navitDBusInterface)
        , DBus::ObjectProxy(con, navitDBusPath, navitDBusDestination.c_str())
    {
    }

    int zoom() {
        return getAttr<int>("zoom", *this);
    }

    void zoomBy(int factor) {
        call("zoom", *this, factor);
    }
};

struct NavitDBusPrivate {

    std::shared_ptr<DBus::Connection> con;
    std::shared_ptr<NavitDBusObjectProxy> object;
    DBus::BusDispatcher dispatcher;

    bool m_threadRunning = false;
    std::thread m_thread;
};

NavitDBus::NavitDBus()
    : d(new NavitDBusPrivate)
{
}

NavitDBus::~NavitDBus()
{
    stop();
    if (d->m_thread.joinable()) {
        d->m_thread.join();
    }
}

bool NavitDBus::start()
{
    DBus::default_dispatcher = &d->dispatcher;
    d->con.reset(new DBus::Connection{ DBus::Connection::SessionBus() });
    d->object.reset(new NavitDBusObjectProxy(*(d->con.get())));
    d->m_thread = std::move(std::thread([]() {
    }));
    return true;
}

void NavitDBus::stop()
{
    nDebug() << "Stopping Navit DBus client";
    d->con->disconnect();
    d->m_threadRunning = false;
}

void NavitDBus::moveBy(double x, double y)
{
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

} // namespace NXE
