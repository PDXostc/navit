#ifndef NXE_DBUSCONTROLLER_H
#define NXE_DBUSCONTROLLER_H

#include <memory>

namespace DBus {
class Connection;
}

namespace NXE {

struct DBusControllerPrivate;

class DBusController
{
public:
    DBusController();
    ~DBusController();

    DBus::Connection& connection() const noexcept;

    void start();
    void stop();
private:
    std::shared_ptr<DBusControllerPrivate> d;
};

} // namespace NXE

#endif // NXE_DBUSCONTROLLER_H
