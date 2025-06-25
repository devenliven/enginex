#ifndef UTILITIES_LOGGER_H_
#define UTILITIES_LOGGER_H_

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h> // Or other sinks as needed
#include <spdlog/sinks/basic_file_sink.h>
#include <memory>  // For std::shared_ptr
#include <string>  // For std::string
#include <utility> // For std::forward
#include <iostream>

#define LOG_TRACE(fmt, ...) Logger::getInstance().trace(fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) Logger::getInstance().debug(fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) Logger::getInstance().info(fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) Logger::getInstance().warn(fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) Logger::getInstance().error(fmt, ##__VA_ARGS__)
#define LOG_CRITICAL(fmt, ...) Logger::getInstance().critical(fmt, ##__VA_ARGS__)

#define SET_LOG_LEVEL(level) Logger::getInstance().set_level(level)

class Logger
{
  private:
    std::shared_ptr<spdlog::logger> logger_;

    Logger()
    {
        try {

            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_pattern("[%Y-%m-%d %H:%M] [%^%l%$] %v");
            console_sink->set_level(spdlog::level::debug);

            auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/log.txt", true);
            file_sink->set_pattern("[%Y-%m-%d %H:%M] [%^%l%$] %v");
            file_sink->set_level(spdlog::level::trace);

            std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
            logger_ = std::make_shared<spdlog::logger>("default", sinks.begin(), sinks.end());

            spdlog::register_logger(logger_);

            logger_->set_level(spdlog::level::debug);
            logger_->set_pattern("[%Y-%m-%d %H:%M][%^%l%$] %v");
            logger_->flush_on(spdlog::level::warn);

        } catch (const spdlog::spdlog_ex& ex) {
            std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        }
    }

    ~Logger() {}

  public:
    Logger(const Logger&)            = delete;
    Logger& operator=(const Logger&) = delete;

    Logger(Logger&&)            = delete;
    Logger& operator=(Logger&&) = delete;

    static Logger& getInstance()
    {
        static Logger instance;
        return instance;
    }

    template <typename... Args> void trace(spdlog::format_string_t<Args...> fmt, Args&&... args)
    {
        if (logger_) { // Check if logger was initialized successfully
            logger_->trace(fmt, std::forward<Args>(args)...);
        }
    }

    template <typename... Args> void debug(spdlog::format_string_t<Args...> fmt, Args&&... args)
    {
        if (logger_) {
            logger_->debug(fmt, std::forward<Args>(args)...);
        }
    }

    template <typename... Args> void info(spdlog::format_string_t<Args...> fmt, Args&&... args)
    {
        if (logger_) {
            logger_->info(fmt, std::forward<Args>(args)...);
        }
    }

    template <typename... Args> void warn(spdlog::format_string_t<Args...> fmt, Args&&... args)
    {
        if (logger_) {
            logger_->warn(fmt, std::forward<Args>(args)...);
        }
    }

    template <typename... Args> void error(spdlog::format_string_t<Args...> fmt, Args&&... args)
    {
        if (logger_) {
            logger_->error(fmt, std::forward<Args>(args)...);
        }
    }

    template <typename... Args> void critical(spdlog::format_string_t<Args...> fmt, Args&&... args)
    {
        if (logger_) {
            logger_->critical(fmt, std::forward<Args>(args)...);
        }
    }
};

#endif // UTILITIES_LOGGER_H_