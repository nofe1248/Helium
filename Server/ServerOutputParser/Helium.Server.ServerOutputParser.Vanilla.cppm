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
        RE2 timestamp_and_loglevel_regex{R"(\[\d{2}:\d{2}:\d{2}\] \[[a-zA-Z ]+\/(INFO|WARN|ERROR|FATAL|DEBUG)\]: )", RE2::Quiet};
        int hour = 0, minute = 0, second = 0;
        std::string thread_name, log_level;
        std::string preprocessed_output = raw_output;

        try
        {
            if (not RE2::PartialMatch(preprocessed_output, timestamp_and_loglevel_regex, &hour, &minute, &second))
            {
                vanilla_logger->error("Server output preprocessing failed (PartialMatch)");
                return std::nullopt;
            }
            if (not RE2::Replace(&preprocessed_output, timestamp_and_loglevel_regex, ""))
            {
                vanilla_logger->error("Server output preprocessing failed (Replace)");
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

        vanilla_logger->debug("preprocess matched : {} {} {} {} {} preprocessed:({})", hour, minute, second, thread_name, log_level, preprocessed_output);

        return std::make_tuple(preprocessed_output,
                               PreprocessedInfo{
                                   .timestamp = ServerOutputInfoTimeStamp{.hour = hour, .minute = minute, .second = second},
                                     .log_level = log_level
        });
    }

    constexpr auto parseServerOutput(this auto &&self, std::string const &raw_output) noexcept -> std::optional<ServerOutputInfo>
    {
    }

    constexpr auto parsePlayerJoined(this auto &&self, std::string const &raw_output) noexcept -> std::optional<std::string>
    {
    }

    constexpr auto parsePlayerLeft(this auto &&self, std::string const &raw_output) noexcept -> std::optional<std::string>
    {
    }

    constexpr auto parseServerVersion(this auto &&self, std::string const &raw_output) noexcept -> std::optional<std::string>
    {
    }

    constexpr auto parseServerAddress(this auto &&self, std::string const &raw_output) noexcept -> std::optional<std::string>
    {
    }

    constexpr auto testServerStartupDone(this auto &&self, std::string const &raw_output) noexcept -> bool
    {
    }

    constexpr auto testRCONStarted(this auto &&self, std::string const &raw_output) noexcept -> bool
    {
    }

    constexpr auto testServerStopping(this auto &&self, std::string const &raw_output) noexcept -> bool
    {
    }
};
} // namespace helium::server