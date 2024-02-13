/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <string>
#include <memory>
#include <vector>
#include <format>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>

export module Helium.Logger.LoggerImpl;

import Helium.Base;

export namespace helium::logger {
	enum class LogLevel {
		trace = spdlog::level::trace,
		debug = spdlog::level::debug,
		info = spdlog::level::info,
		warn = spdlog::level::warn,
		error = spdlog::level::err,
		critical = spdlog::level::critical
	};

    class LoggerImpl {
    private:
        std::string name_;
	    std::string thread_;
		std::shared_ptr<spdlog::logger> logger_ptr_;

		[[nodiscard]] static auto initLogger(std::string const& name) -> std::shared_ptr<spdlog::logger> {
			static auto stdout_sink
				{std::make_shared<spdlog::sinks::stdout_color_sink_mt>()};
			static auto file_sink
				{std::make_shared<spdlog::sinks::daily_file_sink_mt>("./logs/helium-log.log", 0, 0)};
			auto vec = std::vector<spdlog::sink_ptr>{stdout_sink, file_sink};
			auto logger_ptr = std::make_shared<spdlog::logger>(name, begin(vec), end(vec));
			logger_ptr->set_pattern("[%Y-%m-%d %T.%e] [%^%l%$] %v");
			spdlog::register_logger(logger_ptr);
			return logger_ptr;
		}
	public:
        explicit LoggerImpl(std::string_view const name, std::string_view const thread)
			:	name_(name), thread_(thread), logger_ptr_(LoggerImpl::initLogger(this->name_)) {}

        [[nodiscard]] static auto getLogger(std::string_view const name, std::string_view const thread)
                -> std::shared_ptr<LoggerImpl> {
            auto logger_ptr = std::make_shared<LoggerImpl>(name, thread);
            return logger_ptr;
		}

		template <typename ... Args>
		auto log(LogLevel log_level, std::string_view fmt_str, Args&& ... fmt_args) const -> void {
			this->logger_ptr_->log(
				static_cast<spdlog::level::level_enum>(log_level),
				std::format(
                    "[{}/{}] {}",
                    this->name_,
                    this->thread_,
                    std::vformat(
                        fmt_str,
                        std::make_format_args(
                            std::forward<Args>(fmt_args)...
                        )
                    )
				)
			);
		}

		template <typename ... Args>
		auto trace(std::string_view fmt_str, Args&& ... fmt_args) const -> void {
			this->log(LogLevel::trace, fmt_str, std::forward<Args>(fmt_args)...);
		}

		template <typename ... Args>
		auto debug(std::string_view fmt_str, Args&& ... fmt_args) const -> void {
			this->log(LogLevel::debug, fmt_str, std::forward<Args>(fmt_args)...);
		}

		template <typename ... Args>
		auto info(std::string_view fmt_str, Args&& ... fmt_args) const -> void {
			this->log(LogLevel::info, fmt_str, std::forward<Args>(fmt_args)...);
		}

		template <typename ... Args>
		auto warn(std::string_view fmt_str, Args&& ... fmt_args) const -> void {
			this->log(LogLevel::warn, fmt_str, std::forward<Args>(fmt_args)...);
		}

		template <typename ... Args>
		auto error(std::string_view fmt_str, Args&& ... fmt_args) const -> void {
			this->log(LogLevel::error, fmt_str, std::forward<Args>(fmt_args)...);
		}

		template <typename ... Args>
		auto critical(std::string_view fmt_str, Args&& ... fmt_args) const -> void {
			this->log(LogLevel::critical, fmt_str, std::forward<Args>(fmt_args)...);
		}

		template <typename ... Args>
		auto logw(LogLevel log_level, std::wstring_view fmt_str, Args&& ... fmt_args) const -> void {
			this->logger_ptr_->log(
				static_cast<spdlog::level::level_enum>(log_level),
				std::vformat(
					fmt_str,
					std::make_wformat_args(
						std::forward<Args>(fmt_args)...
					)
				)
			);
		}

		template <typename ... Args>
		auto tracew(std::wstring_view fmt_str, Args&& ... fmt_args) const -> void {
			this->logw(LogLevel::trace, fmt_str, std::forward<Args>(fmt_args)...);
		}

		template <typename ... Args>
		auto debugw(std::wstring_view fmt_str, Args&& ... fmt_args) const -> void {
			this->logw(LogLevel::debug, fmt_str, std::forward<Args>(fmt_args)...);
		}

		template <typename ... Args>
		auto infow(std::wstring_view fmt_str, Args&& ... fmt_args) const -> void {
			this->logw(LogLevel::info, fmt_str, std::forward<Args>(fmt_args)...);
		}

		template <typename ... Args>
		auto warnw(std::wstring_view fmt_str, Args&& ... fmt_args) const -> void {
			this->logw(LogLevel::warn, fmt_str, std::forward<Args>(fmt_args)...);
		}

		template <typename ... Args>
		auto errorw(std::wstring_view fmt_str, Args&& ... fmt_args) const -> void {
			this->logw(LogLevel::error, fmt_str, std::forward<Args>(fmt_args)...);
		}

		template <typename ... Args>
		auto criticalw(std::wstring_view fmt_str, Args&& ... fmt_args) const -> void {
			this->logw(LogLevel::critical, fmt_str, std::forward<Args>(fmt_args)...);
		}
	};
}