#include "spdlog/spdlog.h"
#include "test.h"

int main()
{
    // Set global log level.
    spdlog::set_level(static_cast<spdlog::level::level_enum>(SPDLOG_ACTIVE_LEVEL));

    SPDLOG_TRACE("TRACE");
    SPDLOG_DEBUG("DEBUG");
    SPDLOG_INFO("INFO");
    SPDLOG_WARN("WARN");
    SPDLOG_ERROR("ERROR");
    SPDLOG_CRITICAL("CRITICAL");

    test();

    return 0;
}