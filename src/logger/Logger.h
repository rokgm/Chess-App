#pragma once

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>

#include <memory>

namespace chessAi
{

class Logger
{
public:
    /**
     * Create a file logger.
     *
     * @param path Path to where log file is created.
     */
    static void Init(const std::string& path);

    static std::shared_ptr<spdlog::logger>& getLogger()
    {
        return s_logger;
    }

private:
    inline static std::shared_ptr<spdlog::logger> s_logger = nullptr;
};

} // namespace chessAi

#if DEBUG
    #define CHESS_LOG_TRACE(...) ::chessAi::Logger::getLogger()->trace(__VA_ARGS__)
    #define CHESS_LOG_DEBUG(...) ::chessAi::Logger::getLogger()->debug(__VA_ARGS__)
#else
    #define CHESS_LOG_TRACE(...)
    #define CHESS_LOG_DEBUG(...)
#endif

#define CHESS_LOG_INFO(...) ::chessAi::Logger::getLogger()->info(__VA_ARGS__)
#define CHESS_LOG_WARN(...) ::chessAi::Logger::getLogger()->warn(__VA_ARGS__)
#define CHESS_LOG_ERROR(...) ::chessAi::Logger::getLogger()->error(__VA_ARGS__)
#define CHESS_LOG_CRITICAL(...) ::chessAi::Logger::getLogger()->critical(__VA_ARGS__)