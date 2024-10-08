/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <string>
#include <optional>
#include <tuple>

#include <re2/re2.h>

#define FWD(x) ::std::forward<decltype(x)>(x)

export module Helium.Server.ServerOutputParser.CatServer;

import Helium.Base;
import Helium.Server.ServerOutputParser.Utils;
import Helium.Server.ServerOutputParser.ServerOutputInfo;
import Helium.Utils.RText;

export namespace helium::server
{
using RText = utils::rtext::RText;
using RTextList = utils::rtext::RTextList;
class CatServerOutputParser final : public base::HeliumObject
{
public:
    constexpr auto getParserName(this auto &&self) noexcept -> std::string
    {
        return "CatServerOutputParser";
    }

    constexpr auto getSendMessageCommand(this auto &&self, std::string const &target, std::string const &info) noexcept -> std::string
    {
        return std::format("tellraw {} {{\"text\":{}}}", target, info);
    }

    constexpr auto getSendMessageCommand(this auto &&self, std::string const &target, RText const &info) noexcept -> std::string
    {
        return std::format("tellraw {} {}", target, info.toJSONString());
    }

    constexpr auto getSendMessageCommand(this auto &&self, std::string const &target, RTextList const &info) noexcept -> std::string
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

    constexpr auto getBroadcastMessageCommand(this auto &&self, RTextList const &info) noexcept -> std::string
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
        RE2 const timestamp_and_loglevel_regex{R"(\[(\d{2}):(\d{2}):(\d{2})\] \[(.+)\/(DEBUG|INFO|FINE|WARN|ERROR|FATAL)\]: )"};
        int hour = 0, minute = 0, second = 0;
        std::string thread_name, log_level;
        std::string_view preprocessed_output_view{raw_output};

        if (RE2::Consume(&preprocessed_output_view, timestamp_and_loglevel_regex, &hour, &minute, &second, &thread_name, &log_level))
        {
            return std::make_tuple(
                std::string{
                    preprocessed_output_view
            },
                PreprocessedInfo{.timestamp = ServerOutputInfoTimeStamp{.hour = hour, .minute = minute, .second = second}, .log_level = log_level});
        }

        return std::nullopt;
    }

    constexpr auto parseServerOutput(this auto &&self, std::string const &raw_output) noexcept -> std::optional<ServerOutputInfo>
    {
        auto preprocessed_output_tuple = FWD(self).preprocessServerOutput(raw_output);
        if (not preprocessed_output_tuple.has_value())
        {
            return std::nullopt;
        }
        auto [preprocessed_output, preprocessed_info] = preprocessed_output_tuple.value();

        if (auto player_join_opt = FWD(self).parsePlayerJoined(preprocessed_output); player_join_opt.has_value())
        {
            return ServerOutputInfo(ServerOutputInfoType::PLAYER_JOINED, preprocessed_info, raw_output, preprocessed_output, player_join_opt.value());
        }

        if (auto player_left_opt = FWD(self).parsePlayerLeft(preprocessed_output); player_left_opt.has_value())
        {
            return ServerOutputInfo(ServerOutputInfoType::PLAYER_LEFT, preprocessed_info, raw_output, preprocessed_output, player_left_opt.value());
        }

        if (auto player_message_opt = FWD(self).parsePlayerMessage(preprocessed_output); player_message_opt.has_value())
        {
            return ServerOutputInfo(ServerOutputInfoType::PLAYER_MESSAGE, preprocessed_info, raw_output, preprocessed_output,
                                    player_message_opt.value());
        }

        if (auto server_address_opt = FWD(self).parseServerAddress(preprocessed_output); server_address_opt.has_value())
        {
            return ServerOutputInfo(ServerOutputInfoType::SERVER_ADDRESS, preprocessed_info, raw_output, preprocessed_output,
                                    server_address_opt.value());
        }

        if (auto server_version_opt = FWD(self).parseServerVersion(preprocessed_output); server_version_opt.has_value())
        {
            return ServerOutputInfo(ServerOutputInfoType::SERVER_VERSION, preprocessed_info, raw_output, preprocessed_output,
                                    server_version_opt.value());
        }

        if (FWD(self).testServerStartupDone(preprocessed_output))
        {
            return ServerOutputInfo(ServerOutputInfoType::SERVER_STARTED, preprocessed_info, raw_output, preprocessed_output);
        }

        if (FWD(self).testServerStopping(preprocessed_output))
        {
            return ServerOutputInfo(ServerOutputInfoType::SERVER_STOPPING, preprocessed_info, raw_output, preprocessed_output);
        }

        if (FWD(self).testRCONStarted(preprocessed_output))
        {
            return ServerOutputInfo(ServerOutputInfoType::RCON_STARTED, preprocessed_info, raw_output, preprocessed_output);
        }

        return ServerOutputInfo(ServerOutputInfoType::SERVER_OUTPUT, preprocessed_info, raw_output, preprocessed_output);
    }

    constexpr auto parsePlayerMessage(this auto &&self, std::string const &preprocessed_output) noexcept -> std::optional<PlayerMessage>
    {
        RE2 const message_regex{R"((\[Not Secure\] )?<([^>]+)> (.*))"};
        std::string not_secure, player_name, message;

        if (RE2::FullMatch(preprocessed_output, message_regex, &not_secure, &player_name, &message))
        {
            bool is_secure = not_secure.empty();
            if (verifyPlayerName(player_name))
            {
                return PlayerMessage{.is_secure = is_secure, .player_name = player_name, .player_message = message};
            }
        }

        return std::nullopt;
    }

    constexpr auto parsePlayerJoined(this auto &&self, std::string const &preprocessed_output) noexcept -> std::optional<PlayerJoined>
    {
        RE2 const player_join_regex{R"(([^\[]+)\[(.*?)] logged in with entity id (\d+) at \(([0-9.-]+), ([0-9.-]+), ([0-9.-]+)\))"};
        std::string player_name, raw_address, ip;
        int entity_id = 0, port = 0;
        double x = 0.0f, y = 0.0f, z = 0.0f;

        if (RE2::FullMatch(preprocessed_output, player_join_regex, &player_name, &raw_address, &entity_id, &x, &y, &z))
        {
            if (verifyPlayerName(player_name))
            {
                if (raw_address.front() == '/')
                {
                    RE2 const address_regex{R"((\S+):(\d+))"};
                    raw_address.erase(raw_address.begin());
                    RE2::FullMatch(raw_address, address_regex, &ip, &port);
                }
                if (raw_address == "local")
                {
                    ip = "127.0.0.1";
                }
                if (raw_address == "IP hidden")
                {
                    return PlayerJoined{
                        .player_name = player_name, .entity_id = entity_id, .position = Position{.x = x, .y = y, .z = z},
                                .address = std::nullopt
                    };
                }
                return PlayerJoined{
                    .player_name = player_name,
                    .entity_id = entity_id,
                    .position = Position{.x = x, .y = y, .z = z},
                    .address = IPAddress{.ip = ip, .port = port}
                };
            }
        }

        return std::nullopt;
    }

    constexpr auto parsePlayerLeft(this auto &&self, std::string const &preprocessed_output) noexcept -> std::optional<PlayerLeft>
    {
        RE2 const player_left_regex{R"(([^ ]+) left the game)"};
        std::string stripped = cleanMinecraftColorCode(preprocessed_output);
        std::string player_name;

        if (RE2::FullMatch(stripped, player_left_regex, &player_name))
        {
            if (verifyPlayerName(player_name))
            {
                return PlayerLeft{.player_name = player_name};
            }
        }

        return std::nullopt;
    }

    constexpr auto parseServerVersion(this auto &&self, std::string const &preprocessed_output) noexcept -> std::optional<ServerVersion>
    {
        RE2 const version_regex{R"(Starting minecraft server version (.+))"};
        std::string version;

        if (RE2::FullMatch(preprocessed_output, version_regex, &version))
        {
            return ServerVersion{.version = version};
        }

        return std::nullopt;
    }

    constexpr auto parseServerAddress(this auto &&self, std::string const &preprocessed_output) noexcept -> std::optional<ServerAddress>
    {
        RE2 const address_regex{R"(Starting Minecraft server on (\S+):(\d+))"};
        std::string ip;
        int port = 0;

        if (RE2::FullMatch(preprocessed_output, address_regex, &ip, &port))
        {
            return ServerAddress{.ip = ip, .port = port};
        }

        return std::nullopt;
    }

    constexpr auto testServerStartupDone(this auto &&self, std::string const &preprocessed_output) noexcept -> bool
    {
        RE2 const started_regex{R"(Done \([0-9.]+s\)! For help, type "help"( or "\?")?)"};
        return RE2::FullMatch(preprocessed_output, started_regex);
    }

    constexpr auto testRCONStarted(this auto &&self, std::string const &preprocessed_output) noexcept -> bool
    {
        RE2 const rcon_regex{R"(RCON running on [\w.]+:\d+)"};
        return RE2::FullMatch(preprocessed_output, rcon_regex);
    }

    constexpr auto testServerStopping(this auto &&self, std::string const &preprocessed_output) noexcept -> bool
    {
        return preprocessed_output == "Stopping server";
    }
};
}