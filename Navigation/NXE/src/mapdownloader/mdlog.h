#ifndef LOG_H
#define LOG_H

#include <spdlog/spdlog.h>

#ifndef __FILENAME__
#define __FILENAME__ __BASE_FILE__
#endif

#define mdTrace() spdlog::get("md")->trace() << __FILENAME__ << "@" << __LINE__ << " "
#define mdDebug() spdlog::get("md")->debug() << __FILENAME__ << "@" << __LINE__ << " "
#define mdInfo() spdlog::get("md")->info() << __FILENAME__ << "@" << __LINE__ << " "
#define mdError() spdlog::get("md")->error() << __FILENAME__ << "@" << __LINE__ << " "
#define mdFatal() spdlog::get("md")->critical()

#endif // LOG_H

