/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <string>
#include <optional>
#include <tuple>

#include <pybind11/chrono.h>
#include <pybind11/embed.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>

export module Helium.Server.BindingHelper;

import Helium.Server.ServerInstance;
import Helium.Server.ServerOutputParser;
import Helium.Utils.RText;

namespace py = pybind11;

using RText = helium::utils::rtext::RText;

export namespace helium::server::binding
{
class ServerInstanceBindingHelper
{
};

class ArclightServerOutputParserBindingHelper
{
    ArclightServerOutputParser parser_;

public:
    auto getParserName() noexcept -> std::string
    {
        return this->parser_.getParserName();
    }

    auto getSendMessageCommand(std::string const &target, std::string const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }
    auto getSendMessageCommand(std::string const &target, RText const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }

    auto getBroadcastMessageCommand(std::string const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RText const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }

    auto getStopCommand() noexcept -> std::string
    {
        return this->parser_.getStopCommand();
    }

    auto preprocessServerOutput(std::string const &raw_output) noexcept -> std::optional<std::tuple<std::string, PreprocessedInfo>>
    {
        return this->parser_.preprocessServerOutput(raw_output);
    }

    auto parseServerOutput(std::string const &raw_output) noexcept -> std::optional<ServerOutputInfo>
    {
        return this->parser_.parseServerOutput(raw_output);
    }

    auto parsePlayerJoined(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parsePlayerJoined(raw_output);
    }

    auto parsePlayerLeft(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parsePlayerLeft(raw_output);
    }

    auto parseServerVersion(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parseServerVersion(raw_output);
    }

    auto parseServerAddress(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parseServerAddress(raw_output);
    }

    auto testServerStartupDone(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testServerStartupDone(raw_output);
    }

    auto testRCONStarted(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testRCONStarted(raw_output);
    }

    auto testServerStopping(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testServerStopping(raw_output);
    }
};
class Beta18ServerOutputParserBindingHelper
{
    Beta18ServerOutputParser parser_;

public:
    auto getParserName() noexcept -> std::string
    {
        return this->parser_.getParserName();
    }

    auto getSendMessageCommand(std::string const &target, std::string const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }
    auto getSendMessageCommand(std::string const &target, RText const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }

    auto getBroadcastMessageCommand(std::string const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RText const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }

    auto getStopCommand() noexcept -> std::string
    {
        return this->parser_.getStopCommand();
    }

    auto preprocessServerOutput(std::string const &raw_output) noexcept -> std::optional<std::tuple<std::string, PreprocessedInfo>>
    {
        return this->parser_.preprocessServerOutput(raw_output);
    }

    auto parseServerOutput(std::string const &raw_output) noexcept -> std::optional<ServerOutputInfo>
    {
        return this->parser_.parseServerOutput(raw_output);
    }

    auto parsePlayerJoined(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parsePlayerJoined(raw_output);
    }

    auto parsePlayerLeft(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parsePlayerLeft(raw_output);
    }

    auto parseServerVersion(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parseServerVersion(raw_output);
    }

    auto parseServerAddress(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parseServerAddress(raw_output);
    }

    auto testServerStartupDone(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testServerStartupDone(raw_output);
    }

    auto testRCONStarted(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testRCONStarted(raw_output);
    }

    auto testServerStopping(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testServerStopping(raw_output);
    }
};
class BukkitServerOutputParserBindingHelper
{
    BukkitServerOutputParser parser_;

public:
    auto getParserName() noexcept -> std::string
    {
        return this->parser_.getParserName();
    }

    auto getSendMessageCommand(std::string const &target, std::string const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }
    auto getSendMessageCommand(std::string const &target, RText const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }

    auto getBroadcastMessageCommand(std::string const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RText const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }

    auto getStopCommand() noexcept -> std::string
    {
        return this->parser_.getStopCommand();
    }

    auto preprocessServerOutput(std::string const &raw_output) noexcept -> std::optional<std::tuple<std::string, PreprocessedInfo>>
    {
        return this->parser_.preprocessServerOutput(raw_output);
    }

    auto parseServerOutput(std::string const &raw_output) noexcept -> std::optional<ServerOutputInfo>
    {
        return this->parser_.parseServerOutput(raw_output);
    }

    auto parsePlayerJoined(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parsePlayerJoined(raw_output);
    }

    auto parsePlayerLeft(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parsePlayerLeft(raw_output);
    }

    auto parseServerVersion(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parseServerVersion(raw_output);
    }

    auto parseServerAddress(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parseServerAddress(raw_output);
    }

    auto testServerStartupDone(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testServerStartupDone(raw_output);
    }

    auto testRCONStarted(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testRCONStarted(raw_output);
    }

    auto testServerStopping(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testServerStopping(raw_output);
    }
};
class BukkitLegacyServerOutputParserBindingHelper
{
    BukkitLegacyServerOutputParser parser_;

public:
    auto getParserName() noexcept -> std::string
    {
        return this->parser_.getParserName();
    }

    auto getSendMessageCommand(std::string const &target, std::string const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }
    auto getSendMessageCommand(std::string const &target, RText const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }

    auto getBroadcastMessageCommand(std::string const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RText const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }

    auto getStopCommand() noexcept -> std::string
    {
        return this->parser_.getStopCommand();
    }

    auto preprocessServerOutput(std::string const &raw_output) noexcept -> std::optional<std::tuple<std::string, PreprocessedInfo>>
    {
        return this->parser_.preprocessServerOutput(raw_output);
    }

    auto parseServerOutput(std::string const &raw_output) noexcept -> std::optional<ServerOutputInfo>
    {
        return this->parser_.parseServerOutput(raw_output);
    }

    auto parsePlayerJoined(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parsePlayerJoined(raw_output);
    }

    auto parsePlayerLeft(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parsePlayerLeft(raw_output);
    }

    auto parseServerVersion(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parseServerVersion(raw_output);
    }

    auto parseServerAddress(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parseServerAddress(raw_output);
    }

    auto testServerStartupDone(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testServerStartupDone(raw_output);
    }

    auto testRCONStarted(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testRCONStarted(raw_output);
    }

    auto testServerStopping(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testServerStopping(raw_output);
    }
};
class BungeecordServerOutputParserBindingHelper
{
    BungeecordServerOutputParser parser_;

public:
    auto getParserName() noexcept -> std::string
    {
        return this->parser_.getParserName();
    }

    auto getSendMessageCommand(std::string const &target, std::string const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }
    auto getSendMessageCommand(std::string const &target, RText const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }

    auto getBroadcastMessageCommand(std::string const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RText const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }

    auto getStopCommand() noexcept -> std::string
    {
        return this->parser_.getStopCommand();
    }

    auto preprocessServerOutput(std::string const &raw_output) noexcept -> std::optional<std::tuple<std::string, PreprocessedInfo>>
    {
        return this->parser_.preprocessServerOutput(raw_output);
    }

    auto parseServerOutput(std::string const &raw_output) noexcept -> std::optional<ServerOutputInfo>
    {
        return this->parser_.parseServerOutput(raw_output);
    }

    auto parsePlayerJoined(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parsePlayerJoined(raw_output);
    }

    auto parsePlayerLeft(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parsePlayerLeft(raw_output);
    }

    auto parseServerVersion(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parseServerVersion(raw_output);
    }

    auto parseServerAddress(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parseServerAddress(raw_output);
    }

    auto testServerStartupDone(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testServerStartupDone(raw_output);
    }

    auto testRCONStarted(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testRCONStarted(raw_output);
    }

    auto testServerStopping(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testServerStopping(raw_output);
    }
};
class CatServerOutputParserBindingHelper
{
    CatServerOutputParser parser_;

public:
    auto getParserName() noexcept -> std::string
    {
        return this->parser_.getParserName();
    }

    auto getSendMessageCommand(std::string const &target, std::string const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }
    auto getSendMessageCommand(std::string const &target, RText const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }

    auto getBroadcastMessageCommand(std::string const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RText const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }

    auto getStopCommand() noexcept -> std::string
    {
        return this->parser_.getStopCommand();
    }

    auto preprocessServerOutput(std::string const &raw_output) noexcept -> std::optional<std::tuple<std::string, PreprocessedInfo>>
    {
        return this->parser_.preprocessServerOutput(raw_output);
    }

    auto parseServerOutput(std::string const &raw_output) noexcept -> std::optional<ServerOutputInfo>
    {
        return this->parser_.parseServerOutput(raw_output);
    }

    auto parsePlayerJoined(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parsePlayerJoined(raw_output);
    }

    auto parsePlayerLeft(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parsePlayerLeft(raw_output);
    }

    auto parseServerVersion(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parseServerVersion(raw_output);
    }

    auto parseServerAddress(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parseServerAddress(raw_output);
    }

    auto testServerStartupDone(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testServerStartupDone(raw_output);
    }

    auto testRCONStarted(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testRCONStarted(raw_output);
    }

    auto testServerStopping(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testServerStopping(raw_output);
    }
};
class ForgeServerOutputParserBindingHelper
{
    ForgeServerOutputParser parser_;

public:
    auto getParserName() noexcept -> std::string
    {
        return this->parser_.getParserName();
    }

    auto getSendMessageCommand(std::string const &target, std::string const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }
    auto getSendMessageCommand(std::string const &target, RText const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }

    auto getBroadcastMessageCommand(std::string const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RText const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }

    auto getStopCommand() noexcept -> std::string
    {
        return this->parser_.getStopCommand();
    }

    auto preprocessServerOutput(std::string const &raw_output) noexcept -> std::optional<std::tuple<std::string, PreprocessedInfo>>
    {
        return this->parser_.preprocessServerOutput(raw_output);
    }

    auto parseServerOutput(std::string const &raw_output) noexcept -> std::optional<ServerOutputInfo>
    {
        return this->parser_.parseServerOutput(raw_output);
    }

    auto parsePlayerJoined(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parsePlayerJoined(raw_output);
    }

    auto parsePlayerLeft(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parsePlayerLeft(raw_output);
    }

    auto parseServerVersion(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parseServerVersion(raw_output);
    }

    auto parseServerAddress(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parseServerAddress(raw_output);
    }

    auto testServerStartupDone(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testServerStartupDone(raw_output);
    }

    auto testRCONStarted(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testRCONStarted(raw_output);
    }

    auto testServerStopping(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testServerStopping(raw_output);
    }
};
class PythonCustomParserWrapperBindingHelper
{
    PythonCustomParserWrapper parser_;

public:
    auto getParserName() noexcept -> std::string
    {
        return this->parser_.getParserName();
    }

    auto getSendMessageCommand(std::string const &target, std::string const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }
    auto getSendMessageCommand(std::string const &target, RText const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }

    auto getBroadcastMessageCommand(std::string const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RText const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }

    auto getStopCommand() noexcept -> std::string
    {
        return this->parser_.getStopCommand();
    }

    auto preprocessServerOutput(std::string const &raw_output) noexcept -> std::optional<std::tuple<std::string, PreprocessedInfo>>
    {
        return this->parser_.preprocessServerOutput(raw_output);
    }

    auto parseServerOutput(std::string const &raw_output) noexcept -> std::optional<ServerOutputInfo>
    {
        return this->parser_.parseServerOutput(raw_output);
    }

    auto parsePlayerJoined(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parsePlayerJoined(raw_output);
    }

    auto parsePlayerLeft(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parsePlayerLeft(raw_output);
    }

    auto parseServerVersion(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parseServerVersion(raw_output);
    }

    auto parseServerAddress(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parseServerAddress(raw_output);
    }

    auto testServerStartupDone(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testServerStartupDone(raw_output);
    }

    auto testRCONStarted(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testRCONStarted(raw_output);
    }

    auto testServerStopping(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testServerStopping(raw_output);
    }
};
class VanillaServerOutputParserBindingHelper
{
    VanillaServerOutputParser parser_;

public:
    auto getParserName() noexcept -> std::string
    {
        return this->parser_.getParserName();
    }

    auto getSendMessageCommand(std::string const &target, std::string const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }
    auto getSendMessageCommand(std::string const &target, RText const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }

    auto getBroadcastMessageCommand(std::string const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RText const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }

    auto getStopCommand() noexcept -> std::string
    {
        return this->parser_.getStopCommand();
    }

    auto preprocessServerOutput(std::string const &raw_output) noexcept -> std::optional<std::tuple<std::string, PreprocessedInfo>>
    {
        return this->parser_.preprocessServerOutput(raw_output);
    }

    auto parseServerOutput(std::string const &raw_output) noexcept -> std::optional<ServerOutputInfo>
    {
        return this->parser_.parseServerOutput(raw_output);
    }

    auto parsePlayerJoined(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parsePlayerJoined(raw_output);
    }

    auto parsePlayerLeft(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parsePlayerLeft(raw_output);
    }

    auto parseServerVersion(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parseServerVersion(raw_output);
    }

    auto parseServerAddress(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parseServerAddress(raw_output);
    }

    auto testServerStartupDone(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testServerStartupDone(raw_output);
    }

    auto testRCONStarted(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testRCONStarted(raw_output);
    }

    auto testServerStopping(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testServerStopping(raw_output);
    }
};
class VelocityServerOutputParserBindingHelper
{
    VelocityServerOutputParser parser_;

public:
    auto getParserName() noexcept -> std::string
    {
        return this->parser_.getParserName();
    }

    auto getSendMessageCommand(std::string const &target, std::string const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }
    auto getSendMessageCommand(std::string const &target, RText const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }

    auto getBroadcastMessageCommand(std::string const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RText const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }

    auto getStopCommand() noexcept -> std::string
    {
        return this->parser_.getStopCommand();
    }

    auto preprocessServerOutput(std::string const &raw_output) noexcept -> std::optional<std::tuple<std::string, PreprocessedInfo>>
    {
        return this->parser_.preprocessServerOutput(raw_output);
    }

    auto parseServerOutput(std::string const &raw_output) noexcept -> std::optional<ServerOutputInfo>
    {
        return this->parser_.parseServerOutput(raw_output);
    }

    auto parsePlayerJoined(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parsePlayerJoined(raw_output);
    }

    auto parsePlayerLeft(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parsePlayerLeft(raw_output);
    }

    auto parseServerVersion(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parseServerVersion(raw_output);
    }

    auto parseServerAddress(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parseServerAddress(raw_output);
    }

    auto testServerStartupDone(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testServerStartupDone(raw_output);
    }

    auto testRCONStarted(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testRCONStarted(raw_output);
    }

    auto testServerStopping(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testServerStopping(raw_output);
    }
};
class WaterfallServerOutputParserBindingHelper
{
    WaterfallServerOutputParser parser_;

public:
    auto getParserName() noexcept -> std::string
    {
        return this->parser_.getParserName();
    }

    auto getSendMessageCommand(std::string const &target, std::string const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }
    auto getSendMessageCommand(std::string const &target, RText const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }

    auto getBroadcastMessageCommand(std::string const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RText const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }

    auto getStopCommand() noexcept -> std::string
    {
        return this->parser_.getStopCommand();
    }

    auto preprocessServerOutput(std::string const &raw_output) noexcept -> std::optional<std::tuple<std::string, PreprocessedInfo>>
    {
        return this->parser_.preprocessServerOutput(raw_output);
    }

    auto parseServerOutput(std::string const &raw_output) noexcept -> std::optional<ServerOutputInfo>
    {
        return this->parser_.parseServerOutput(raw_output);
    }

    auto parsePlayerJoined(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parsePlayerJoined(raw_output);
    }

    auto parsePlayerLeft(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parsePlayerLeft(raw_output);
    }

    auto parseServerVersion(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parseServerVersion(raw_output);
    }

    auto parseServerAddress(std::string const &raw_output) noexcept -> std::optional<std::string>
    {
        return this->parser_.parseServerAddress(raw_output);
    }

    auto testServerStartupDone(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testServerStartupDone(raw_output);
    }

    auto testRCONStarted(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testRCONStarted(raw_output);
    }

    auto testServerStopping(std::string const &raw_output) noexcept -> bool
    {
        return this->parser_.testServerStopping(raw_output);
    }
};
} // namespace helium::server::binding