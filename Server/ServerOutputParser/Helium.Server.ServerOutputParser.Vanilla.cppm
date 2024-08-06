/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <optional>
#include <string>
#include <tuple>

#include <re2/re2.h>

#define FWD(x) ::std::forward<decltype(x)>(x)

export module Helium.Server.ServerOutputParser.Vanilla;

import Helium.Base;
import Helium.Server.ServerOutputParser.Utils;
import Helium.Server.ServerOutputParser.ServerOutputInfo;
import Helium.Utils.RText;
import Helium.Logger;

namespace helium::server
{
auto vanilla_logger = logger::SharedLogger::getSharedLogger("ServerOutputParser", "Vanilla");
}

export namespace helium::server
{
using RText = utils::rtext::RText;
class VanillaServerOutputParser final : public base::HeliumObject
{
public:
    constexpr auto getParserName(this auto &&self) noexcept -> std::string
    {
        return "VanillaServerOutputParser";
    }

    constexpr auto getSendMessageCommand(this auto &&self, std::string const &target, std::string const &info) noexcept -> std::string
    {
        return std::format("tellraw {} {{\"text\":{}}}", target, info);
    }

    constexpr auto getSendMessageCommand(this auto &&self, std::string const &target, RText const &info) noexcept -> std::string
    {
        return std::format("tellraw {} {}", target, info.toJSONString());
    }

    constexpr auto getBroadcastMessageCommand(this auto &&self, std::string const &info) noexcept -> std::string
    {
        return std::format("tellraw @e {{\"text\":{}}}", info);
    }

    constexpr auto getBroadcastMessageCommand(this auto &&self, RText const &info) noexcept -> std::string
    {
        return std::format("tellraw @e {}", info.toJSONString());
    }

    constexpr auto getStopCommand(this auto &&self) noexcept -> std::string
    {
        return "stop";
    }

    constexpr auto preprocessServerOutput(this auto &&self, std::string const &raw_output) noexcept
        -> std::optional<std::tuple<std::string, PreprocessedInfo>>
    {
        RE2 timestamp_and_loglevel_regex{R"(\[(\d{2}):(\d{2}):(\d{2})\] \[([0-9a-zA-Z -]+)\/(INFO|WARN|ERROR|FATAL|DEBUG)\]: )", RE2::Quiet};
        int hour = 0, minute = 0, second = 0;
        std::string thread_name, log_level;
        std::string_view preprocessed_output_view{raw_output};

        try
        {
            if (not RE2::Consume(&preprocessed_output_view, timestamp_and_loglevel_regex, &hour, &minute, &second, &thread_name, &log_level))
            {
                return std::nullopt;
            }
        }
        catch (std::exception const &e)
        {
            vanilla_logger->error("Server output preprocessing failed due to an exception: {}", e.what());
            return std::nullopt;
        }
        catch (...)
        {
            vanilla_logger->error("Server output preprocessing failed due to an unknown exception");
            return std::nullopt;
        }

        return std::make_tuple(
            std::string{
                preprocessed_output_view
        },
            PreprocessedInfo{.timestamp = ServerOutputInfoTimeStamp{.hour = hour, .minute = minute, .second = second}, .log_level = log_level});
    }

    constexpr auto parseServerOutput(this auto &&self, std::string const &raw_output) noexcept -> std::optional<ServerOutputInfo>
    {
        auto preprocessed_output_tuple = FWD(self).preprocessServerOutput(raw_output);
        if (not preprocessed_output_tuple.has_value())
        {
            return std::nullopt;
        }
        auto [preprocessed_output, preprocessed_info] = preprocessed_output_tuple.value();

        if (auto player_name_opt = FWD(self).parsePlayerJoined(preprocessed_output); preprocessed_output_tuple.has_value())
        {
            return ServerOutputInfo{ServerOutputInfoType::PLAYER_JOINED, preprocessed_info, raw_output, preprocessed_output, player_name_opt};
        }

        if (auto player_name_opt = FWD(self).parsePlayerLeft(preprocessed_output); preprocessed_output_tuple.has_value())
        {
            return ServerOutputInfo{ServerOutputInfoType::PLAYER_LEFT, preprocessed_info, raw_output, preprocessed_output, player_name_opt};
        }

        if (auto player_name_opt = FWD(self).parsePlayerMessage(preprocessed_output); preprocessed_output_tuple.has_value())
        {
            return ServerOutputInfo{ServerOutputInfoType::PLAYER_MESSAGE, preprocessed_info, raw_output, preprocessed_output,
                                    std::get<1>(player_name_opt.value())};
        }

        return std::nullopt;
    }

    constexpr auto parsePlayerMessage(this auto &&self, std::string const &preprocessed_output) noexcept
        -> std::optional<std::tuple<std::string, std::string>>
    {
        return std::nullopt;
    }

    constexpr auto parsePlayerJoined(this auto &&self, std::string const &preprocessed_output) noexcept -> std::optional<std::string>
    {
        return std::nullopt;
    }

    constexpr auto parsePlayerLeft(this auto &&self, std::string const &preprocessed_output) noexcept -> std::optional<std::string>
    {
        return std::nullopt;
    }

    constexpr auto parseServerVersion(this auto &&self, std::string const &preprocessed_output) noexcept -> std::optional<std::string>
    {
        return std::nullopt;
    }

    constexpr auto parseServerAddress(this auto &&self, std::string const &preprocessed_output) noexcept -> std::optional<std::string>
    {
        return std::nullopt;
    }

    constexpr auto testServerStartupDone(this auto &&self, std::string const &preprocessed_output) noexcept -> bool
    {
        return false;
    }

    constexpr auto testRCONStarted(this auto &&self, std::string const &preprocessed_output) noexcept -> bool
    {
        return false;
    }

    constexpr auto testServerStopping(this auto &&self, std::string const &preprocessed_output) noexcept -> bool
    {
        return false;
    }
};
} // namespace helium::server