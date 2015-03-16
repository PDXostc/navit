#ifndef LOG_H
#define LOG_H

#include <spdlog/spdlog.h>

#define nTrace() spdlog::get("nxe")->trace() << __FILENAME__ << "@" << __LINE__ << " "
#define nDebug() spdlog::get("nxe")->debug() << __FILENAME__ << "@" << __LINE__ << " "
#define nInfo() spdlog::get("nxe")->info() << __FILENAME__ << "@" << __LINE__ << " "
#define nError() spdlog::get("nxe")->error()
#define nFatal() spdlog::get("nxe")->critical()

#define perfLog(Measurement) spdlog::get("perf")->info() << Measurement
#endif // LOG_H

