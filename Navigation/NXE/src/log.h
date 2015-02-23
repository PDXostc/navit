#ifndef LOG_H
#define LOG_H

#include <spdlog/spdlog.h>

#define nDebug() spdlog::get("nxe_logger")->debug()
#define nInfo() spdlog::get("nxe_logger")->info()
#define nFatal() spdlog::get("nxe_logger")->warn()

#endif // LOG_H

