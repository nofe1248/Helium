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

export module Helium.Server.ServerOutputParser.Beta18;

import Helium.Base;
import Helium.Server.ServerOutputParser.Utils;
import Helium.Server.ServerOutputParser.ServerOutputInfo;
import Helium.Utils.RText;

export namespace helium::server
{
using RText = utils::rtext::RText;
class Beta18ServerOutputParser final : public base::HeliumObject
{
public:
    constexpr auto getParserName(this auto &&self) noexcept -> std::string
    {
        return "Beta18ServerOutputParser";
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
        return "";
    }

    constexpr auto getBroadcastMessageCommand(this auto &&self, RText const &info) noexcept -> std::string
    {
        return "";
    }

    constexpr auto getStopCommand(this auto &&self) noexcept -> std::string
    {
        return "";
    }

    constexpr auto preprocessServerOutput(this auto &&self, std::string const &raw_output) noexcept -> std::optional<std::tuple<std::string, PreprocessedInfo>>
    {
        return std::nullopt;
    }

    constexpr auto parseServerOutput(this auto &&self, std::string const &raw_output) noexcept -> std::optional<ServerOutputInfo>
    {
        return std::nullopt;
    }

    constexpr auto parsePlayerJoined(this auto &&self, std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return std::nullopt;
    }

    constexpr auto parsePlayerLeft(this auto &&self, std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return std::nullopt;
    }

    constexpr auto parseServerVersion(this auto &&self, std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return std::nullopt;
    }

    constexpr auto parseServerAddress(this auto &&self, std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return std::nullopt;
    }

    constexpr auto testServerStartupDone(this auto &&self, std::string const &raw_output) noexcept -> bool
    {
        return false;
    }

    constexpr auto testRCONStarted(this auto &&self, std::string const &raw_output) noexcept -> bool
    {
        return false;
    }

    constexpr auto testServerStopping(this auto &&self, std::string const &raw_output) noexcept -> bool
    {
        return false;
    }
};
}