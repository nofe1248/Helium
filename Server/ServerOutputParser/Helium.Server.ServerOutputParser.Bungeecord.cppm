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

export module Helium.Server.ServerOutputParser.Bungeecord;

import Helium.Base;
import Helium.Server.ServerOutputParser.Utils;
import Helium.Server.ServerOutputParser.ServerOutputInfo;
import Helium.Utils.RText;

export namespace helium::server
{
using RText = utils::rtext::RText;
class BungeecordServerOutputParser final : public base::HeliumObject
{
public:
    constexpr auto getParserName(this auto &&self) noexcept -> std::string
    {
        return "BungeecordServerOutputParser";
    }

    constexpr auto getSendMessageCommand(this auto &&self, std::string const &target, std::string const &info) noexcept -> std::string
    {
        return "";
    }

    constexpr auto getSendMessageCommand(this auto &&self, std::string const &target, RText const &info) noexcept -> std::string
    {
        return "";
    }

    constexpr auto getBroadcastMessageCommand(this auto &&self, std::string const &info) noexcept -> std::string
    {
        return std::format("alertraw {}", info);
    }

    constexpr auto getBroadcastMessageCommand(this auto &&self, RText const &info) noexcept -> std::string
    {
        return std::format("alertraw {}", info.toJSONString());
    }

    constexpr auto getStopCommand(this auto &&self) noexcept -> std::string
    {
        return "end";
    }

    constexpr auto preprocessServerOutput(this auto &&self, std::string const &raw_output) noexcept
        -> std::optional<std::tuple<std::string, PreprocessedInfo>>
    {
        RE2 preprocess_regex{R"((\d+):(\d+):(\d+) \[([^]]+)\] )"};
        int hour = 0, minute = 0, second = 0;
        std::string log_level;
        std::string_view preprocessed_string_view{raw_output};

        if (RE2::Consume(&preprocessed_string_view, preprocess_regex, &hour, &minute, &second, &log_level))
        {
            return std::make_tuple(
                std::string{
                    preprocessed_string_view
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
        RE2 message_regex{R"((\[Not Secure\] )?<([^>]+)> (.*))"};
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
        RE2 player_join_regex{R"(\[([^,]+),\/(.*):(\d+)] <-> InitialHandler has connected)"};
        std::string player_name, ip;
        int port = 0;

        if (RE2::FullMatch(preprocessed_output, player_join_regex, &player_name, &ip, &port))
        {
            if (verifyPlayerName(player_name))
            {
                return PlayerJoined{
                    .player_name = player_name, .entity_id = 0, .position = {},
                            .address = IPAddress{.ip = ip, .port = port}
                };
            }
        }

        return std::nullopt;
    }

    constexpr auto parsePlayerLeft(this auto &&self, std::string const &preprocessed_output) noexcept -> std::optional<PlayerLeft>
    {
        RE2 player_left_regex{R"(\[([^]]+)\] -> UpstreamBridge has disconnected)"};
        std::string player_name;

        if (RE2::FullMatch(preprocessed_output, player_left_regex, &player_name))
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
        return std::nullopt;
    }

    constexpr auto parseServerAddress(this auto &&self, std::string const &preprocessed_output) noexcept -> std::optional<ServerAddress>
    {
        RE2 server_address_regex{R"(Listening on \/(\S+):(\d+))"};
        std::string ip;
        int port = 0;

        if (RE2::FullMatch(preprocessed_output, server_address_regex, &ip, &port))
        {
            return ServerAddress{.ip = ip, .port = port};
        }

        return std::nullopt;
    }

    constexpr auto testServerStartupDone(this auto &&self, std::string const &preprocessed_output) noexcept -> bool
    {
        RE2 server_address_regex{R"(Listening on \/(\S+):(\d+))"};
        return RE2::FullMatch(preprocessed_output, server_address_regex);
    }

    constexpr auto testRCONStarted(this auto &&self, std::string const &preprocessed_output) noexcept -> bool
    {
        RE2 server_address_regex{R"(Listening on \/(\S+):(\d+))"};
        return RE2::FullMatch(preprocessed_output, server_address_regex);
    }

    constexpr auto testServerStopping(this auto &&self, std::string const &preprocessed_output) noexcept -> bool
    {
        RE2 server_stopping_regex{R"(Closing listener \[id: .+, L:[\d:\/]+])"};
        return RE2::FullMatch(preprocessed_output, server_stopping_regex);
    }
};
} // namespace helium::server