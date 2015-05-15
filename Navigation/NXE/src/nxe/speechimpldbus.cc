#include "speechimpldbus.h"
#include "log.h"
#include "dbuscontroller.h"
#include <dbus-c++/dbus.h>
#include "dbus_helpers.hpp"

#include <functional>

namespace NXE {
const std::string srsDBusDestination = "org.tizen.srs";
const std::string srsDBusPath = "/tts";
const std::string srsDBusInterface = "org.tizen.srs";

struct SRSDBusObjectProxy : public ::DBus::InterfaceProxy, public ::DBus::ObjectProxy {
    SRSDBusObjectProxy(::DBus::Connection& connection)
        : ::DBus::InterfaceProxy(srsDBusInterface)
        , ::DBus::ObjectProxy(connection, srsDBusPath, srsDBusInterface.c_str())
    {}
};

struct SpeechImplDBusPrivate {
    SpeechImplDBusPrivate(DBusController& ctrl)
        : object (ctrl.connection())
    {}
    SRSDBusObjectProxy object;
};

SpeechImplDBus::SpeechImplDBus(DBusController& ctrl)
    : d(new SpeechImplDBusPrivate (ctrl))
{
}

SpeechImplDBus::~SpeechImplDBus()
{
}

void SpeechImplDBus::say(const std::string& command)
{
    try {
        DBusHelpers::call("synthesize", d->object, command, std::string{"english"});
    } catch( const std::exception& ex) {
        nError() << "Error in speech = " << ex.what();
    }
}

} // namespace NXE
