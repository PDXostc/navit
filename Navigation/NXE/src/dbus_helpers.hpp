#ifndef DBUS_HELPERS_HPP
#define DBUS_HELPERS_HPP

#include <dbus-c++/interface.h>
#include <dbus-c++/message.h>
#include "log.h"

namespace NXE {
namespace DBus {

template<typename R>
R getAttr(const std::string &attrName, ::DBus::InterfaceProxy &proxy) {
    ::DBus::CallMessage call;
    ::DBus::MessageIter it = call.writer();
    call.member("get_attr");
    it << attrName;
    ::DBus::Message ret = proxy.invoke_method(call);

    if (ret.is_error()) {
        throw std::runtime_error("Unable to call zoom");
    }

    std::string attr;
    ::DBus::MessageIter retIter = ret.reader();
    retIter >> attr ;
    ::DBus::Variant v;
    retIter >> v;
    R value = static_cast<R>(v);
    nDebug() << attrName << "= " << value;

    return value;
}

void unpack(::DBus::MessageIter &)
{}

template<typename T>
void processOne(::DBus::MessageIter &it, T t)
{
    it << t;
}

template<typename T, typename ...Args>
void unpack(::DBus::MessageIter &it, T t, Args... args)
{
    processOne(it,t);
    unpack(it,args...);
}

template<typename ...Args>
void call(const std::string &methodName, ::DBus::InterfaceProxy &proxy, Args... attr)
{
    nDebug() << "Calling dbus " << methodName;
    ::DBus::CallMessage call;
    ::DBus::MessageIter it = call.writer();
    call.member(methodName.c_str());
    nDebug() << "Signature " << call.signature();
    unpack(it, attr...);
    ::DBus::Message ret = proxy.invoke_method(call);
    nDebug() << "After " << methodName << " call";
    if (ret.is_error()) {
        nFatal() << "Unable to call " << methodName;
        throw std::runtime_error("Unable to call" + methodName);
    }
}

} // DBus
} // NXE

#endif // DBUS_HELPERS_HPP

