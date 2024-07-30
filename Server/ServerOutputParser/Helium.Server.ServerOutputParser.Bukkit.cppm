/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <string>

#include <re2/re2.h>

#define FWD(x) ::std::forward<decltype(x)>(x)

export module Helium.Server.ServerOutputParser.Bukkit;

import Helium.Base;
import Helium.Server.ServerOutputParser.Utils;

export namespace helium::server
{
class BukkitServerOutputParser final : public base::HeliumObject
{
public:
    constexpr auto getParserName(this auto &&self) noexcept -> std::string
    {
        return "BukkitServerOutputParser";
    }

    constexpr auto getSendMessageCommand(this auto &&self, std::string const &target, std::string const &info) noexcept -> std::string
    {
    }

    constexpr auto getBroadcastMessageCommand(this auto &&self, std::string const &info) noexcept -> std::string
    {
    }

    constexpr auto getStopCommand(this auto &&self) noexcept -> std::string
    {
    }

    constexpr auto preprocessServerOutput(this auto &&self, std::string const &raw_output) noexcept -> std::string
    {
    }

    constexpr auto parseServerOutput(this auto &&self, std::string const &raw_output) noexcept -> ServerOutputInfo
    {
    }

    constexpr auto parsePlayerJoined(this auto &&self, std::string const &raw_output) noexcept -> std::string
    {
    }

    constexpr auto parsePlayerLeft(this auto &&self, std::string const &raw_output) noexcept -> std::string
    {
    }

    constexpr auto parseServerVersion(this auto &&self, std::string const &raw_output) noexcept -> std::string
    {
    }

    constexpr auto parseServerAddress(this auto &&self, std::string const &raw_output) noexcept -> std::string
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
}