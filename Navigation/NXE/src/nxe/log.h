#ifndef LOG_H
#define LOG_H

#include <spdlog/spdlog.h>

#ifndef __FILENAME__
#define __FILENAME__ __BASE_FILE__
#endif

#define nTrace() spdlog::get("nxe")->trace() << __FILENAME__ << "@" << __LINE__ << " "
#define nDebug() spdlog::get("nxe")->debug() << __FILENAME__ << "@" << __LINE__ << " "
#define nInfo() spdlog::get("nxe")->info() << __FILENAME__ << "@" << __LINE__ << " "
#define nError() spdlog::get("nxe")->error() << __FILENAME__ << "@" << __LINE__ << " "
#define nFatal() spdlog::get("nxe")->critical()

#define dbusTrace() spdlog::get("dbus")->trace() << __FILENAME__ << "@" << __LINE__ << " "
#define dbusDebug() spdlog::get("dbus")->debug() << __FILENAME__ << "@" << __LINE__ << " "
#define dbusInfo() spdlog::get("dbus")->info() << __FILENAME__ << "@" << __LINE__ << " "
#define dbusError() spdlog::get("dbus")->error() << __FILENAME__ << "@" << __LINE__ << " "
#define dbusFatal() spdlog::get("dbus")->critical()

namespace NXE {
struct PointClicked;
struct Position;
}

std::ostream& operator<<(std::ostream& os, const NXE::Position&);
std::ostream& operator<<(std::ostream& os, const NXE::PointClicked&);

#endif // LOG_H
