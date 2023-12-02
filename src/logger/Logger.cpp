#include "Logger.h"

#include <spdlog/async.h>

namespace chessAi
{

void Logger::Init(const std::string& path)
{
    s_logger = spdlog::rotating_logger_mt<spdlog::async_factory>("chessAi_logger", path,
                                                                 1048576 * 5, 3, true);
    s_logger->set_level(spdlog::level::trace);
    spdlog::flush_every(std::chrono::milliseconds(100));
}

} // namespace chessAi