#include "test.h"

#include "spdlog/spdlog.h"

void test()
{
    SPDLOG_TRACE("TEST_TRACE");
    SPDLOG_DEBUG("TEST_DEBUG");
    SPDLOG_INFO("TEST_INFO");
    SPDLOG_WARN("TEST_WARN");
    SPDLOG_ERROR("TEST_ERROR");
    SPDLOG_CRITICAL("TEST_CRITICAL");
}