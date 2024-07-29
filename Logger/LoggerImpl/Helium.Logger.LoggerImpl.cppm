/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <chrono>
#include <format>
#include <memory>
#include <source_location>
#include <string>
#include <vector>

#include <spdlog/async.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

export module Helium.Logger.LoggerImpl;

import Helium.Base;

export namespace helium::logger
{
enum class LogLevel
{
    trace = spdlog::level::trace,
    debug = spdlog::level::debug,
    info = spdlog::level::info,
    warn = spdlog::level::warn,
    error = spdlog::level::err,
    critical = spdlog::level::critical
};

class LoggerImpl
{
private:
    std::string name_;
    std::string thread_;
    std::shared_ptr<spdlog::logger> logger_ptr_;

    [[nodiscard]] static auto initLogger(std::string const &name) -> std::shared_ptr<spdlog::logger>
    {
        static auto stdout_sink{std::make_shared<spdlog::sinks::stdout_color_sink_mt>()};
        static auto file_sink{std::make_shared<spdlog::sinks::daily_file_sink_mt>("./logs/helium-log.log", 0, 0)};
        stdout_sink->set_level(spdlog::level::trace);
        file_sink->set_level(spdlog::level::info);

        auto vec = spdlog::sinks_init_list{stdout_sink, file_sink};

        if (not spdlog::thread_pool())
        {
            spdlog::init_thread_pool(2048, 4);
            spdlog::flush_every(std::chrono::seconds(1));
        }

        auto logger_ptr = std::make_shared<spdlog::async_logger>(name, vec, spdlog::thread_pool());
        logger_ptr->set_pattern("[%Y-%m-%d %T.%f] [%^%=8l%$] %v");

        spdlog::register_logger(logger_ptr);
        return logger_ptr;
    }

public:
    explicit LoggerImpl(std::string_view const name, std::string_view const thread)
        : name_(name), thread_(thread), logger_ptr_(LoggerImpl::initLogger(std::format("{}/{}", this->name_, this->thread_)))
    {
    }

    ~LoggerImpl()
    {
        this->logger_ptr_->flush();
    }

    template <typename... Args>
    auto log(LogLevel log_level, std::string_view fmt_str, Args &&...fmt_args) const -> void
    {
        if constexpr (sizeof...(fmt_args) >= 1)
        {
            this->logger_ptr_->log(
                static_cast<spdlog::level::level_enum>(log_level),
                std::format("[{}/{}] {}", this->name_, this->thread_, std::format(std::runtime_format(fmt_str), std::forward<Args>(fmt_args)...)));
        }
        else
        {
            this->logger_ptr_->log(static_cast<spdlog::level::level_enum>(log_level), std::format("[{}/{}] {}", this->name_, this->thread_, fmt_str));
        }
    }

    template <typename... Args>
    auto log_raw(LogLevel log_level, std::string_view fmt_str, Args &&...fmt_args) const -> void
    {
        if constexpr (sizeof...(fmt_args) >= 1)
        {
            this->logger_ptr_->log(static_cast<spdlog::level::level_enum>(log_level),
                                   std::format(std::runtime_format(fmt_str), std::forward<Args>(fmt_args)...));
        }
        else
        {
            this->logger_ptr_->log(static_cast<spdlog::level::level_enum>(log_level), fmt_str);
        }
    }

    template <typename... Args>
    auto trace(std::string_view fmt_str, Args &&...fmt_args) const -> void
    {
        this->log(LogLevel::trace, fmt_str, std::forward<Args>(fmt_args)...);
    }

    template <typename... Args>
    auto debug(std::string_view fmt_str, Args &&...fmt_args) const -> void
    {
        this->log(LogLevel::debug, fmt_str, std::forward<Args>(fmt_args)...);
    }

    template <typename... Args>
    auto info(std::string_view fmt_str, Args &&...fmt_args) const -> void
    {
        this->log(LogLevel::info, fmt_str, std::forward<Args>(fmt_args)...);
    }

    template <typename... Args>
    auto warn(std::string_view fmt_str, Args &&...fmt_args) const -> void
    {
        this->log(LogLevel::warn, fmt_str, std::forward<Args>(fmt_args)...);
    }

    template <typename... Args>
    auto error(std::string_view fmt_str, Args &&...fmt_args) const -> void
    {
        this->log(LogLevel::error, fmt_str, std::forward<Args>(fmt_args)...);
    }

    template <typename... Args>
    auto critical(std::string_view fmt_str, Args &&...fmt_args) const -> void
    {
        this->log(LogLevel::critical, fmt_str, std::forward<Args>(fmt_args)...);
    }

    auto trace_string(std::string_view str) const -> void
    {
        this->log(LogLevel::trace, "{}", str);
    }

    auto debug_string(std::string_view str) const -> void
    {
        this->log(LogLevel::debug, "{}", str);
    }

    auto info_string(std::string_view str) const -> void
    {
        this->log(LogLevel::info, "{}", str);
    }

    auto warn_string(std::string_view str) const -> void
    {
        this->log(LogLevel::warn, "{}", str);
    }

    auto error_string(std::string_view str) const -> void
    {
        this->log(LogLevel::error, "{}", str);
    }

    auto critical_string(std::string_view str) const -> void
    {
        this->log(LogLevel::critical, "{}", str);
    }

    auto flush() noexcept -> void
    {
        this->logger_ptr_->flush();
    }

    auto enableDebugLog() noexcept -> void
    {
        this->logger_ptr_->set_level(spdlog::level::trace);
    }
    auto disableDebugLog() noexcept -> void
    {
        this->logger_ptr_->set_level(spdlog::level::info);
    }
    static auto enableAllDebugLog() noexcept -> void
    {
        spdlog::set_level(spdlog::level::trace);
    }
    static auto disableAllDebugLog() noexcept -> void
    {
        spdlog::set_level(spdlog::level::info);
    }
};
} // namespace helium::logger
