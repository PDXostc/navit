
#include "log.h"

spdlog::details::line_logger perfLog(const std::string& name)
{
    auto log = spdlog::get("perf") != nullptr ? spdlog::get("perf")->info()
                                              : spdlog::get("nxe")->warn();
    log << name;
    return log;
}
