#include "Logger.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace chessAi
{

void Logger::Init()
{
    if (s_logger != nullptr) {
        return;
    }

    try {
        std::vector<std::shared_ptr<spdlog::sinks::sink>> sinks;
        sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        sinks.emplace_back(
            std::make_shared<spdlog::sinks::basic_file_sink_mt>("chessAi_logger.txt", true));
        sinks[0]->set_level(spdlog::level::trace);
        sinks[1]->set_level(spdlog::level::trace);
        auto logger = std::make_shared<spdlog::logger>("chessAi_logger", begin(sinks), end(sinks));
        s_logger = logger;
        s_logger->set_level(spdlog::level::trace);
        spdlog::flush_every(std::chrono::milliseconds(300));
    }
    catch (const std::exception& ex) {
        CHESS_LOG_ERROR("Logger not created. Exception: {}.", ex.what());
        s_logger = nullptr;
    }
}

std::shared_ptr<spdlog::logger>& Logger::getLogger()
{
    if (s_logger == nullptr) {
        Init();
    }
    return s_logger;
}

} // namespace chessAi