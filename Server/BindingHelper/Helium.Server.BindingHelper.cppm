/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <filesystem>
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
import Helium.Logger;

namespace py = pybind11;
namespace fs = std::filesystem;

using RText = helium::utils::rtext::RText;
using RTextList = helium::utils::rtext::RTextList;

namespace helium::server::binding
{
auto server_binding_logger = logger::SharedLogger::getSharedLogger("Server", "PythonBinding");
}

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
    static auto sendMessage(std::string const &target, RTextList const &info) -> bool
    {
        if (not ServerInstance::getInstancePointer())
        {
            return false;
        }
        return ServerInstance::getInstancePointer()->sendMessage(target, info);
    }
    static auto sendMessage(py::args info_list) -> bool
    {
        if (not ServerInstance::getInstancePointer())
        {
            return false;
        }
        RTextList rtext_list;
        if (info_list.size() <= 1)
        {
            server_binding_logger->error("sendMessage should be called with at least 2 strings");
            return false;
        }
        for (auto it = info_list.begin() + 1; it < info_list.end(); ++it)
        {
            try
            {
                rtext_list.addRText(it->cast<RText>());
            }
            catch (py::error_already_set const &e)
            {
                server_binding_logger->error("Exception when constructing RTextList from py::args: {}", e.what());
            }
        }
        try
        {
            return ServerInstance::getInstancePointer()->sendMessage(info_list.begin()->cast<std::string>(), rtext_list);
        }
        catch (py::error_already_set const &e)
        {
            server_binding_logger->error("Exception when getting message target name from py::args: {}", e.what());
        }
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
    static auto broadcastMessage(RTextList const &info) -> bool
    {
        if (not ServerInstance::getInstancePointer())
        {
            return false;
        }
        return ServerInstance::getInstancePointer()->broadcastMessage(info);
    }
    static auto broadcastMessage(py::args info_list) -> bool
    {
        if (not ServerInstance::getInstancePointer())
        {
            return false;
        }
        RTextList rtext_list;
        if (info_list.size() == 0)
        {
            server_binding_logger->error("broadcastMessage should be called with at least 1 strings");
            return "";
        }
        for (auto const &info : info_list)
        {
            try
            {
                rtext_list.addRText(info.cast<RText>());
            }
            catch (py::error_already_set const &e)
            {
                server_binding_logger->error("Exception when constructing RTextList from py::args: {}", e.what());
            }
        }
        return ServerInstance::getInstancePointer()->broadcastMessage(rtext_list);
    }
    static auto getPath() -> fs::path
    {
        return ServerInstance::getInstancePointer()->getPath();
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
    auto getSendMessageCommand(std::string const &target, RTextList const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }
    auto getSendMessageCommand(py::args info_list) -> std::string
    {
        RTextList rtext_list;
        if (info_list.size() <= 1)
        {
            server_binding_logger->error("getSendMessageCommand should be called with at least 2 strings");
            return "";
        }
        for (auto it = info_list.begin() + 1; it < info_list.end(); ++it)
        {
            try
            {
                rtext_list.addRText(it->cast<RText>());
            }
            catch (py::error_already_set const &e)
            {
                server_binding_logger->error("Exception when constructing RTextList from py::args: {}", e.what());
            }
        }
        try
        {
            return this->parser_.getSendMessageCommand(info_list.begin()->cast<std::string>(), rtext_list);
        }
        catch (py::error_already_set const &e)
        {
            server_binding_logger->error("Exception when getting message target name from py::args: {}", e.what());
        }
    }

    auto getBroadcastMessageCommand(std::string const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RText const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RTextList const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(py::args info_list) -> std::string
    {
        RTextList rtext_list;
        if (info_list.size() == 0)
        {
            server_binding_logger->error("getBroadcastMessageCommand should be called with at least 1 strings");
            return "";
        }
        for (auto const &info : info_list)
        {
            try
            {
                rtext_list.addRText(info.cast<RText>());
            }
            catch (py::error_already_set const &e)
            {
                server_binding_logger->error("Exception when constructing RTextList from py::args: {}", e.what());
            }
        }
        return this->parser_.getBroadcastMessageCommand(rtext_list);
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
    auto getSendMessageCommand(std::string const &target, RTextList const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }
    auto getSendMessageCommand(py::args info_list) -> std::string
    {
        RTextList rtext_list;
        if (info_list.size() <= 1)
        {
            server_binding_logger->error("getSendMessageCommand should be called with at least 2 strings");
            return "";
        }
        for (auto it = info_list.begin() + 1; it < info_list.end(); ++it)
        {
            try
            {
                rtext_list.addRText(it->cast<RText>());
            }
            catch (py::error_already_set const &e)
            {
                server_binding_logger->error("Exception when constructing RTextList from py::args: {}", e.what());
            }
        }
        try
        {
            return this->parser_.getSendMessageCommand(info_list.begin()->cast<std::string>(), rtext_list);
        }
        catch (py::error_already_set const &e)
        {
            server_binding_logger->error("Exception when getting message target name from py::args: {}", e.what());
        }
    }

    auto getBroadcastMessageCommand(std::string const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RText const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RTextList const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(py::args info_list) -> std::string
    {
        RTextList rtext_list;
        if (info_list.size() == 0)
        {
            server_binding_logger->error("getBroadcastMessageCommand should be called with at least 1 strings");
            return "";
        }
        for (auto const &info : info_list)
        {
            try
            {
                rtext_list.addRText(info.cast<RText>());
            }
            catch (py::error_already_set const &e)
            {
                server_binding_logger->error("Exception when constructing RTextList from py::args: {}", e.what());
            }
        }
        return this->parser_.getBroadcastMessageCommand(rtext_list);
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
    auto getSendMessageCommand(std::string const &target, RTextList const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }
    auto getSendMessageCommand(py::args info_list) -> std::string
    {
        RTextList rtext_list;
        if (info_list.size() <= 1)
        {
            server_binding_logger->error("getSendMessageCommand should be called with at least 2 strings");
            return "";
        }
        for (auto it = info_list.begin() + 1; it < info_list.end(); ++it)
        {
            try
            {
                rtext_list.addRText(it->cast<RText>());
            }
            catch (py::error_already_set const &e)
            {
                server_binding_logger->error("Exception when constructing RTextList from py::args: {}", e.what());
            }
        }
        try
        {
            return this->parser_.getSendMessageCommand(info_list.begin()->cast<std::string>(), rtext_list);
        }
        catch (py::error_already_set const &e)
        {
            server_binding_logger->error("Exception when getting message target name from py::args: {}", e.what());
        }
    }

    auto getBroadcastMessageCommand(std::string const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RText const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RTextList const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(py::args info_list) -> std::string
    {
        RTextList rtext_list;
        if (info_list.size() == 0)
        {
            server_binding_logger->error("getBroadcastMessageCommand should be called with at least 1 strings");
            return "";
        }
        for (auto const &info : info_list)
        {
            try
            {
                rtext_list.addRText(info.cast<RText>());
            }
            catch (py::error_already_set const &e)
            {
                server_binding_logger->error("Exception when constructing RTextList from py::args: {}", e.what());
            }
        }
        return this->parser_.getBroadcastMessageCommand(rtext_list);
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
    auto getSendMessageCommand(std::string const &target, RTextList const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }
    auto getSendMessageCommand(py::args info_list) -> std::string
    {
        RTextList rtext_list;
        if (info_list.size() <= 1)
        {
            server_binding_logger->error("getSendMessageCommand should be called with at least 2 strings");
            return "";
        }
        for (auto it = info_list.begin() + 1; it < info_list.end(); ++it)
        {
            try
            {
                rtext_list.addRText(it->cast<RText>());
            }
            catch (py::error_already_set const &e)
            {
                server_binding_logger->error("Exception when constructing RTextList from py::args: {}", e.what());
            }
        }
        try
        {
            return this->parser_.getSendMessageCommand(info_list.begin()->cast<std::string>(), rtext_list);
        }
        catch (py::error_already_set const &e)
        {
            server_binding_logger->error("Exception when getting message target name from py::args: {}", e.what());
        }
    }

    auto getBroadcastMessageCommand(std::string const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RText const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RTextList const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(py::args info_list) -> std::string
    {
        RTextList rtext_list;
        if (info_list.size() == 0)
        {
            server_binding_logger->error("getBroadcastMessageCommand should be called with at least 1 strings");
            return "";
        }
        for (auto const &info : info_list)
        {
            try
            {
                rtext_list.addRText(info.cast<RText>());
            }
            catch (py::error_already_set const &e)
            {
                server_binding_logger->error("Exception when constructing RTextList from py::args: {}", e.what());
            }
        }
        return this->parser_.getBroadcastMessageCommand(rtext_list);
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
    auto getSendMessageCommand(std::string const &target, RTextList const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }
    auto getSendMessageCommand(py::args info_list) -> std::string
    {
        RTextList rtext_list;
        if (info_list.size() <= 1)
        {
            server_binding_logger->error("getSendMessageCommand should be called with at least 2 strings");
            return "";
        }
        for (auto it = info_list.begin() + 1; it < info_list.end(); ++it)
        {
            try
            {
                rtext_list.addRText(it->cast<RText>());
            }
            catch (py::error_already_set const &e)
            {
                server_binding_logger->error("Exception when constructing RTextList from py::args: {}", e.what());
            }
        }
        try
        {
            return this->parser_.getSendMessageCommand(info_list.begin()->cast<std::string>(), rtext_list);
        }
        catch (py::error_already_set const &e)
        {
            server_binding_logger->error("Exception when getting message target name from py::args: {}", e.what());
        }
    }

    auto getBroadcastMessageCommand(std::string const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RText const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RTextList const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(py::args info_list) -> std::string
    {
        RTextList rtext_list;
        if (info_list.size() == 0)
        {
            server_binding_logger->error("getBroadcastMessageCommand should be called with at least 1 strings");
            return "";
        }
        for (auto const &info : info_list)
        {
            try
            {
                rtext_list.addRText(info.cast<RText>());
            }
            catch (py::error_already_set const &e)
            {
                server_binding_logger->error("Exception when constructing RTextList from py::args: {}", e.what());
            }
        }
        return this->parser_.getBroadcastMessageCommand(rtext_list);
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
    auto getSendMessageCommand(std::string const &target, RTextList const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }
    auto getSendMessageCommand(py::args info_list) -> std::string
    {
        RTextList rtext_list;
        if (info_list.size() <= 1)
        {
            server_binding_logger->error("getSendMessageCommand should be called with at least 2 strings");
            return "";
        }
        for (auto it = info_list.begin() + 1; it < info_list.end(); ++it)
        {
            try
            {
                rtext_list.addRText(it->cast<RText>());
            }
            catch (py::error_already_set const &e)
            {
                server_binding_logger->error("Exception when constructing RTextList from py::args: {}", e.what());
            }
        }
        try
        {
            return this->parser_.getSendMessageCommand(info_list.begin()->cast<std::string>(), rtext_list);
        }
        catch (py::error_already_set const &e)
        {
            server_binding_logger->error("Exception when getting message target name from py::args: {}", e.what());
        }
    }

    auto getBroadcastMessageCommand(std::string const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RText const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RTextList const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(py::args info_list) -> std::string
    {
        RTextList rtext_list;
        if (info_list.size() == 0)
        {
            server_binding_logger->error("getBroadcastMessageCommand should be called with at least 1 strings");
            return "";
        }
        for (auto const &info : info_list)
        {
            try
            {
                rtext_list.addRText(info.cast<RText>());
            }
            catch (py::error_already_set const &e)
            {
                server_binding_logger->error("Exception when constructing RTextList from py::args: {}", e.what());
            }
        }
        return this->parser_.getBroadcastMessageCommand(rtext_list);
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
    auto getSendMessageCommand(std::string const &target, RTextList const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }
    auto getSendMessageCommand(py::args info_list) -> std::string
    {
        RTextList rtext_list;
        if (info_list.size() <= 1)
        {
            server_binding_logger->error("getSendMessageCommand should be called with at least 2 strings");
            return "";
        }
        for (auto it = info_list.begin() + 1; it < info_list.end(); ++it)
        {
            try
            {
                rtext_list.addRText(it->cast<RText>());
            }
            catch (py::error_already_set const &e)
            {
                server_binding_logger->error("Exception when constructing RTextList from py::args: {}", e.what());
            }
        }
        try
        {
            return this->parser_.getSendMessageCommand(info_list.begin()->cast<std::string>(), rtext_list);
        }
        catch (py::error_already_set const &e)
        {
            server_binding_logger->error("Exception when getting message target name from py::args: {}", e.what());
        }
    }

    auto getBroadcastMessageCommand(std::string const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RText const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RTextList const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(py::args info_list) -> std::string
    {
        RTextList rtext_list;
        if (info_list.size() == 0)
        {
            server_binding_logger->error("getBroadcastMessageCommand should be called with at least 1 strings");
            return "";
        }
        for (auto const &info : info_list)
        {
            try
            {
                rtext_list.addRText(info.cast<RText>());
            }
            catch (py::error_already_set const &e)
            {
                server_binding_logger->error("Exception when constructing RTextList from py::args: {}", e.what());
            }
        }
        return this->parser_.getBroadcastMessageCommand(rtext_list);
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
    auto getSendMessageCommand(std::string const &target, RTextList const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }
    auto getSendMessageCommand(py::args info_list) -> std::string
    {
        RTextList rtext_list;
        if (info_list.size() <= 1)
        {
            server_binding_logger->error("getSendMessageCommand should be called with at least 2 strings");
            return "";
        }
        for (auto it = info_list.begin() + 1; it < info_list.end(); ++it)
        {
            try
            {
                rtext_list.addRText(it->cast<RText>());
            }
            catch (py::error_already_set const &e)
            {
                server_binding_logger->error("Exception when constructing RTextList from py::args: {}", e.what());
            }
        }
        try
        {
            return this->parser_.getSendMessageCommand(info_list.begin()->cast<std::string>(), rtext_list);
        }
        catch (py::error_already_set const &e)
        {
            server_binding_logger->error("Exception when getting message target name from py::args: {}", e.what());
        }
    }

    auto getBroadcastMessageCommand(std::string const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RText const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RTextList const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(py::args info_list) -> std::string
    {
        RTextList rtext_list;
        if (info_list.size() == 0)
        {
            server_binding_logger->error("getBroadcastMessageCommand should be called with at least 1 strings");
            return "";
        }
        for (auto const &info : info_list)
        {
            try
            {
                rtext_list.addRText(info.cast<RText>());
            }
            catch (py::error_already_set const &e)
            {
                server_binding_logger->error("Exception when constructing RTextList from py::args: {}", e.what());
            }
        }
        return this->parser_.getBroadcastMessageCommand(rtext_list);
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
    auto getSendMessageCommand(std::string const &target, RTextList const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }
    auto getSendMessageCommand(py::args info_list) -> std::string
    {
        RTextList rtext_list;
        if (info_list.size() <= 1)
        {
            server_binding_logger->error("getSendMessageCommand should be called with at least 2 strings");
            return "";
        }
        for (auto it = info_list.begin() + 1; it < info_list.end(); ++it)
        {
            try
            {
                rtext_list.addRText(it->cast<RText>());
            }
            catch (py::error_already_set const &e)
            {
                server_binding_logger->error("Exception when constructing RTextList from py::args: {}", e.what());
            }
        }
        try
        {
            return this->parser_.getSendMessageCommand(info_list.begin()->cast<std::string>(), rtext_list);
        }
        catch (py::error_already_set const &e)
        {
            server_binding_logger->error("Exception when getting message target name from py::args: {}", e.what());
        }
    }

    auto getBroadcastMessageCommand(std::string const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RText const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RTextList const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(py::args info_list) -> std::string
    {
        RTextList rtext_list;
        if (info_list.size() == 0)
        {
            server_binding_logger->error("getBroadcastMessageCommand should be called with at least 1 strings");
            return "";
        }
        for (auto const &info : info_list)
        {
            try
            {
                rtext_list.addRText(info.cast<RText>());
            }
            catch (py::error_already_set const &e)
            {
                server_binding_logger->error("Exception when constructing RTextList from py::args: {}", e.what());
            }
        }
        return this->parser_.getBroadcastMessageCommand(rtext_list);
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
    auto getSendMessageCommand(std::string const &target, RTextList const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }
    auto getSendMessageCommand(py::args info_list) -> std::string
    {
        RTextList rtext_list;
        if (info_list.size() <= 1)
        {
            server_binding_logger->error("getSendMessageCommand should be called with at least 2 strings");
            return "";
        }
        for (auto it = info_list.begin() + 1; it < info_list.end(); ++it)
        {
            try
            {
                rtext_list.addRText(it->cast<RText>());
            }
            catch (py::error_already_set const &e)
            {
                server_binding_logger->error("Exception when constructing RTextList from py::args: {}", e.what());
            }
        }
        try
        {
            return this->parser_.getSendMessageCommand(info_list.begin()->cast<std::string>(), rtext_list);
        }
        catch (py::error_already_set const &e)
        {
            server_binding_logger->error("Exception when getting message target name from py::args: {}", e.what());
        }
    }

    auto getBroadcastMessageCommand(std::string const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RText const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RTextList const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(py::args info_list) -> std::string
    {
        RTextList rtext_list;
        if (info_list.size() == 0)
        {
            server_binding_logger->error("getBroadcastMessageCommand should be called with at least 1 strings");
            return "";
        }
        for (auto const &info : info_list)
        {
            try
            {
                rtext_list.addRText(info.cast<RText>());
            }
            catch (py::error_already_set const &e)
            {
                server_binding_logger->error("Exception when constructing RTextList from py::args: {}", e.what());
            }
        }
        return this->parser_.getBroadcastMessageCommand(rtext_list);
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
    auto getSendMessageCommand(std::string const &target, RTextList const &info) noexcept -> std::string
    {
        return this->parser_.getSendMessageCommand(target, info);
    }
    auto getSendMessageCommand(py::args info_list) -> std::string
    {
        RTextList rtext_list;
        if (info_list.size() <= 1)
        {
            server_binding_logger->error("getSendMessageCommand should be called with at least 2 strings");
            return "";
        }
        for (auto it = info_list.begin() + 1; it < info_list.end(); ++it)
        {
            try
            {
                rtext_list.addRText(it->cast<RText>());
            }
            catch (py::error_already_set const &e)
            {
                server_binding_logger->error("Exception when constructing RTextList from py::args: {}", e.what());
            }
        }
        try
        {
            return this->parser_.getSendMessageCommand(info_list.begin()->cast<std::string>(), rtext_list);
        }
        catch (py::error_already_set const &e)
        {
            server_binding_logger->error("Exception when getting message target name from py::args: {}", e.what());
        }
    }

    auto getBroadcastMessageCommand(std::string const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RText const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(RTextList const &info) noexcept -> std::string
    {
        return this->parser_.getBroadcastMessageCommand(info);
    }
    auto getBroadcastMessageCommand(py::args info_list) -> std::string
    {
        RTextList rtext_list;
        if (info_list.size() == 0)
        {
            server_binding_logger->error("getBroadcastMessageCommand should be called with at least 1 strings");
            return "";
        }
        for (auto const &info : info_list)
        {
            try
            {
                rtext_list.addRText(info.cast<RText>());
            }
            catch (py::error_already_set const &e)
            {
                server_binding_logger->error("Exception when constructing RTextList from py::args: {}", e.what());
            }
        }
        return this->parser_.getBroadcastMessageCommand(rtext_list);
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