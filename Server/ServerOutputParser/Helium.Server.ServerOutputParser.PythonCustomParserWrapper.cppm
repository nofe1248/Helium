/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <string>

#include <re2/re2.h>

#include <pybind11/chrono.h>
#include <pybind11/embed.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>

#define FWD(x) ::std::forward<decltype(x)>(x)

export module Helium.Server.ServerOutputParser.PythonCustomParserWrapper;

import Helium.Base;
import Helium.Server.ServerOutputParser.Utils;
import Helium.Server.ServerOutputParser.ServerOutputInfo;
import Helium.Utils.RText;

namespace py = pybind11;

export namespace helium::server
{
using RText = utils::rtext::RText;
class PythonCustomParserWrapper final : public base::HeliumObject
{
public:
    constexpr auto getParserName(this auto &&self) noexcept -> std::string
    {
        return "PythonCustomParserWrapper";
    }

    constexpr auto getSendMessageCommand(this auto &&self, std::string const &target, std::string const &info) noexcept -> std::string
    {
    }

    constexpr auto getSendMessageCommand(this auto &&self, std::string const &target, RText const &info) noexcept -> std::string
    {
    }

    constexpr auto getBroadcastMessageCommand(this auto &&self, std::string const &info) noexcept -> std::string
    {
    }

    constexpr auto getBroadcastMessageCommand(this auto &&self, RText const &info) noexcept -> std::string
    {
    }

    constexpr auto getStopCommand(this auto &&self) noexcept -> std::string
    {
    }

    constexpr auto preprocessServerOutput(this auto &&self, std::string const &raw_output) noexcept -> std::tuple<std::string, PreprocessedInfo>
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
} // namespace helium::server