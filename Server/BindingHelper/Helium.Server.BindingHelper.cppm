/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <optional>
#include <string>
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
public:
    static auto sendRawInput(std::string const &input) -> bool
    {
        if (not ServerInstance::getInstancePointer())
        {
            return false;
        }
        return ServerInstance::getInstancePointer()->sendRawInput(input);
    }
    static auto start() -> bool
    {
        if (not ServerInstance::getInstancePointer())
        {
            return false;
        }
        return ServerInstance::getInstancePointer()->start();
    }
    static auto stop() -> bool
    {
        if (not ServerInstance::getInstancePointer())
        {
            return false;
        }
        return ServerInstance::getInstancePointer()->stop();
    }
    static auto kill() -> bool
    {
        if (not ServerInstance::getInstancePointer())
        {
            return false;
        }
        return ServerInstance::getInstancePointer()->kill();
    }
    static auto getServerState() -> std::optional<ServerState>
    {
        if (not ServerInstance::getInstancePointer())
        {
            return std::nullopt;
        }
        return ServerInstance::getInstancePointer()->getServerState();
    }
    static auto sendMessage(std::string const &target, std::string const &info) -> bool
    {
        if (not ServerInstance::getInstancePointer())
        {
            return false;
        }
        return ServerInstance::getInstancePointer()->sendMessage(target, info);
    }
    static auto sendMessage(std::string const &target, RText const &info) -> bool
    {
        if (not ServerInstance::getInstancePointer())
        {
            return false;
        }
        return ServerInstance::getInstancePointer()->sendMessage(target, info);
    }
    static auto broadcastMessage(std::string const &info) -> bool
    {
        if (not ServerInstance::getInstancePointer())
        {
            return false;
        }
        return ServerInstance::getInstancePointer()->broadcastMessage(info);
    }
    static auto broadcastMessage(RText const &info) -> bool
    {
        if (not ServerInstance::getInstancePointer())
        {
            return false;
        }
        return ServerInstance::getInstancePointer()->broadcastMessage(info);
    }
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

    auto parsePlayerMessage(std::string const &preprocessed_output) noexcept -> std::optional<PlayerMessage>
    {
        return this->parser_.parsePlayerMessage(preprocessed_output);
    }

    auto parsePlayerJoined(std::string const &preprocessed_output) noexcept -> std::optional<PlayerJoined>
    {
        return this->parser_.parsePlayerJoined(preprocessed_output);
    }

    auto parsePlayerLeft(std::string const &preprocessed_output) noexcept -> std::optional<PlayerLeft>
    {
        return this->parser_.parsePlayerLeft(preprocessed_output);
    }

    auto parseServerVersion(std::string const &preprocessed_output) noexcept -> std::optional<ServerVersion>
    {
        return this->parser_.parseServerVersion(preprocessed_output);
    }

    auto parseServerAddress(std::string const &preprocessed_output) noexcept -> std::optional<ServerAddress>
    {
        return this->parser_.parseServerAddress(preprocessed_output);
    }

    auto testServerStartupDone(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testServerStartupDone(preprocessed_output);
    }

    auto testRCONStarted(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testRCONStarted(preprocessed_output);
    }

    auto testServerStopping(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testServerStopping(preprocessed_output);
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

    auto parsePlayerMessage(std::string const &preprocessed_output) noexcept -> std::optional<PlayerMessage>
    {
        return this->parser_.parsePlayerMessage(preprocessed_output);
    }

    auto parsePlayerJoined(std::string const &preprocessed_output) noexcept -> std::optional<PlayerJoined>
    {
        return this->parser_.parsePlayerJoined(preprocessed_output);
    }

    auto parsePlayerLeft(std::string const &preprocessed_output) noexcept -> std::optional<PlayerLeft>
    {
        return this->parser_.parsePlayerLeft(preprocessed_output);
    }

    auto parseServerVersion(std::string const &preprocessed_output) noexcept -> std::optional<ServerVersion>
    {
        return this->parser_.parseServerVersion(preprocessed_output);
    }

    auto parseServerAddress(std::string const &preprocessed_output) noexcept -> std::optional<ServerAddress>
    {
        return this->parser_.parseServerAddress(preprocessed_output);
    }

    auto testServerStartupDone(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testServerStartupDone(preprocessed_output);
    }

    auto testRCONStarted(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testRCONStarted(preprocessed_output);
    }

    auto testServerStopping(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testServerStopping(preprocessed_output);
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

    auto parsePlayerMessage(std::string const &preprocessed_output) noexcept -> std::optional<PlayerMessage>
    {
        return this->parser_.parsePlayerMessage(preprocessed_output);
    }

    auto parsePlayerJoined(std::string const &preprocessed_output) noexcept -> std::optional<PlayerJoined>
    {
        return this->parser_.parsePlayerJoined(preprocessed_output);
    }

    auto parsePlayerLeft(std::string const &preprocessed_output) noexcept -> std::optional<PlayerLeft>
    {
        return this->parser_.parsePlayerLeft(preprocessed_output);
    }

    auto parseServerVersion(std::string const &preprocessed_output) noexcept -> std::optional<ServerVersion>
    {
        return this->parser_.parseServerVersion(preprocessed_output);
    }

    auto parseServerAddress(std::string const &preprocessed_output) noexcept -> std::optional<ServerAddress>
    {
        return this->parser_.parseServerAddress(preprocessed_output);
    }

    auto testServerStartupDone(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testServerStartupDone(preprocessed_output);
    }

    auto testRCONStarted(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testRCONStarted(preprocessed_output);
    }

    auto testServerStopping(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testServerStopping(preprocessed_output);
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

    auto parsePlayerMessage(std::string const &preprocessed_output) noexcept -> std::optional<PlayerMessage>
    {
        return this->parser_.parsePlayerMessage(preprocessed_output);
    }

    auto parsePlayerJoined(std::string const &preprocessed_output) noexcept -> std::optional<PlayerJoined>
    {
        return this->parser_.parsePlayerJoined(preprocessed_output);
    }

    auto parsePlayerLeft(std::string const &preprocessed_output) noexcept -> std::optional<PlayerLeft>
    {
        return this->parser_.parsePlayerLeft(preprocessed_output);
    }

    auto parseServerVersion(std::string const &preprocessed_output) noexcept -> std::optional<ServerVersion>
    {
        return this->parser_.parseServerVersion(preprocessed_output);
    }

    auto parseServerAddress(std::string const &preprocessed_output) noexcept -> std::optional<ServerAddress>
    {
        return this->parser_.parseServerAddress(preprocessed_output);
    }

    auto testServerStartupDone(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testServerStartupDone(preprocessed_output);
    }

    auto testRCONStarted(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testRCONStarted(preprocessed_output);
    }

    auto testServerStopping(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testServerStopping(preprocessed_output);
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

    auto parsePlayerMessage(std::string const &preprocessed_output) noexcept -> std::optional<PlayerMessage>
    {
        return this->parser_.parsePlayerMessage(preprocessed_output);
    }

    auto parsePlayerJoined(std::string const &preprocessed_output) noexcept -> std::optional<PlayerJoined>
    {
        return this->parser_.parsePlayerJoined(preprocessed_output);
    }

    auto parsePlayerLeft(std::string const &preprocessed_output) noexcept -> std::optional<PlayerLeft>
    {
        return this->parser_.parsePlayerLeft(preprocessed_output);
    }

    auto parseServerVersion(std::string const &preprocessed_output) noexcept -> std::optional<ServerVersion>
    {
        return this->parser_.parseServerVersion(preprocessed_output);
    }

    auto parseServerAddress(std::string const &preprocessed_output) noexcept -> std::optional<ServerAddress>
    {
        return this->parser_.parseServerAddress(preprocessed_output);
    }

    auto testServerStartupDone(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testServerStartupDone(preprocessed_output);
    }

    auto testRCONStarted(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testRCONStarted(preprocessed_output);
    }

    auto testServerStopping(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testServerStopping(preprocessed_output);
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

    auto parsePlayerMessage(std::string const &preprocessed_output) noexcept -> std::optional<PlayerMessage>
    {
        return this->parser_.parsePlayerMessage(preprocessed_output);
    }

    auto parsePlayerJoined(std::string const &preprocessed_output) noexcept -> std::optional<PlayerJoined>
    {
        return this->parser_.parsePlayerJoined(preprocessed_output);
    }

    auto parsePlayerLeft(std::string const &preprocessed_output) noexcept -> std::optional<PlayerLeft>
    {
        return this->parser_.parsePlayerLeft(preprocessed_output);
    }

    auto parseServerVersion(std::string const &preprocessed_output) noexcept -> std::optional<ServerVersion>
    {
        return this->parser_.parseServerVersion(preprocessed_output);
    }

    auto parseServerAddress(std::string const &preprocessed_output) noexcept -> std::optional<ServerAddress>
    {
        return this->parser_.parseServerAddress(preprocessed_output);
    }

    auto testServerStartupDone(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testServerStartupDone(preprocessed_output);
    }

    auto testRCONStarted(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testRCONStarted(preprocessed_output);
    }

    auto testServerStopping(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testServerStopping(preprocessed_output);
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

    auto parsePlayerMessage(std::string const &preprocessed_output) noexcept -> std::optional<PlayerMessage>
    {
        return this->parser_.parsePlayerMessage(preprocessed_output);
    }

    auto parsePlayerJoined(std::string const &preprocessed_output) noexcept -> std::optional<PlayerJoined>
    {
        return this->parser_.parsePlayerJoined(preprocessed_output);
    }

    auto parsePlayerLeft(std::string const &preprocessed_output) noexcept -> std::optional<PlayerLeft>
    {
        return this->parser_.parsePlayerLeft(preprocessed_output);
    }

    auto parseServerVersion(std::string const &preprocessed_output) noexcept -> std::optional<ServerVersion>
    {
        return this->parser_.parseServerVersion(preprocessed_output);
    }

    auto parseServerAddress(std::string const &preprocessed_output) noexcept -> std::optional<ServerAddress>
    {
        return this->parser_.parseServerAddress(preprocessed_output);
    }

    auto testServerStartupDone(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testServerStartupDone(preprocessed_output);
    }

    auto testRCONStarted(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testRCONStarted(preprocessed_output);
    }

    auto testServerStopping(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testServerStopping(preprocessed_output);
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

    auto parsePlayerMessage(std::string const &preprocessed_output) noexcept -> std::optional<PlayerMessage>
    {
        return this->parser_.parsePlayerMessage(preprocessed_output);
    }

    auto parsePlayerJoined(std::string const &preprocessed_output) noexcept -> std::optional<PlayerJoined>
    {
        return this->parser_.parsePlayerJoined(preprocessed_output);
    }

    auto parsePlayerLeft(std::string const &preprocessed_output) noexcept -> std::optional<PlayerLeft>
    {
        return this->parser_.parsePlayerLeft(preprocessed_output);
    }

    auto parseServerVersion(std::string const &preprocessed_output) noexcept -> std::optional<ServerVersion>
    {
        return this->parser_.parseServerVersion(preprocessed_output);
    }

    auto parseServerAddress(std::string const &preprocessed_output) noexcept -> std::optional<ServerAddress>
    {
        return this->parser_.parseServerAddress(preprocessed_output);
    }

    auto testServerStartupDone(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testServerStartupDone(preprocessed_output);
    }

    auto testRCONStarted(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testRCONStarted(preprocessed_output);
    }

    auto testServerStopping(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testServerStopping(preprocessed_output);
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

    auto parsePlayerMessage(std::string const &preprocessed_output) noexcept -> std::optional<PlayerMessage>
    {
        return this->parser_.parsePlayerMessage(preprocessed_output);
    }

    auto parsePlayerJoined(std::string const &preprocessed_output) noexcept -> std::optional<PlayerJoined>
    {
        return this->parser_.parsePlayerJoined(preprocessed_output);
    }

    auto parsePlayerLeft(std::string const &preprocessed_output) noexcept -> std::optional<PlayerLeft>
    {
        return this->parser_.parsePlayerLeft(preprocessed_output);
    }

    auto parseServerVersion(std::string const &preprocessed_output) noexcept -> std::optional<ServerVersion>
    {
        return this->parser_.parseServerVersion(preprocessed_output);
    }

    auto parseServerAddress(std::string const &preprocessed_output) noexcept -> std::optional<ServerAddress>
    {
        return this->parser_.parseServerAddress(preprocessed_output);
    }

    auto testServerStartupDone(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testServerStartupDone(preprocessed_output);
    }

    auto testRCONStarted(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testRCONStarted(preprocessed_output);
    }

    auto testServerStopping(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testServerStopping(preprocessed_output);
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

    auto parsePlayerMessage(std::string const &preprocessed_output) noexcept -> std::optional<PlayerMessage>
    {
        return this->parser_.parsePlayerMessage(preprocessed_output);
    }

    auto parsePlayerJoined(std::string const &preprocessed_output) noexcept -> std::optional<PlayerJoined>
    {
        return this->parser_.parsePlayerJoined(preprocessed_output);
    }

    auto parsePlayerLeft(std::string const &preprocessed_output) noexcept -> std::optional<PlayerLeft>
    {
        return this->parser_.parsePlayerLeft(preprocessed_output);
    }

    auto parseServerVersion(std::string const &preprocessed_output) noexcept -> std::optional<ServerVersion>
    {
        return this->parser_.parseServerVersion(preprocessed_output);
    }

    auto parseServerAddress(std::string const &preprocessed_output) noexcept -> std::optional<ServerAddress>
    {
        return this->parser_.parseServerAddress(preprocessed_output);
    }

    auto testServerStartupDone(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testServerStartupDone(preprocessed_output);
    }

    auto testRCONStarted(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testRCONStarted(preprocessed_output);
    }

    auto testServerStopping(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testServerStopping(preprocessed_output);
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

    auto parsePlayerMessage(std::string const &preprocessed_output) noexcept -> std::optional<PlayerMessage>
    {
        return this->parser_.parsePlayerMessage(preprocessed_output);
    }

    auto parsePlayerJoined(std::string const &preprocessed_output) noexcept -> std::optional<PlayerJoined>
    {
        return this->parser_.parsePlayerJoined(preprocessed_output);
    }

    auto parsePlayerLeft(std::string const &preprocessed_output) noexcept -> std::optional<PlayerLeft>
    {
        return this->parser_.parsePlayerLeft(preprocessed_output);
    }

    auto parseServerVersion(std::string const &preprocessed_output) noexcept -> std::optional<ServerVersion>
    {
        return this->parser_.parseServerVersion(preprocessed_output);
    }

    auto parseServerAddress(std::string const &preprocessed_output) noexcept -> std::optional<ServerAddress>
    {
        return this->parser_.parseServerAddress(preprocessed_output);
    }

    auto testServerStartupDone(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testServerStartupDone(preprocessed_output);
    }

    auto testRCONStarted(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testRCONStarted(preprocessed_output);
    }

    auto testServerStopping(std::string const &preprocessed_output) noexcept -> bool
    {
        return this->parser_.testServerStopping(preprocessed_output);
    }
};
} // namespace helium::server::binding