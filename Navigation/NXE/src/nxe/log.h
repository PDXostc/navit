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

spdlog::details::line_logger perfLog(const std::string& name);
#endif // LOG_H
