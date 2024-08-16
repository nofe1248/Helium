/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <string>

export module Helium.BuiltinCommands.Implementations.Server;

import Helium.Commands.CommandContext;
import Helium.Server.ServerInstance;
import Helium.BuiltinCommands.Implementations.Utils;
import Helium.Logger;

namespace helium::commands
{
auto server_logger = logger::SharedLogger::getSharedLogger("BuiltinCommands", "Server");
}

export namespace helium::commands
{
auto serverStart(CommandContext const &ctx) -> void
{
    if (server::ServerInstance::getInstancePointer())
    {
        if (server::ServerInstance::getInstancePointer()->start())
        {
            sendMessageTo(ctx, server_logger, logger::LogLevel::info, "Server started");
            return;
        }
    }
    sendMessageTo(ctx, server_logger, logger::LogLevel::error, "Failed to start the server");
}
auto serverStop(CommandContext const &ctx) -> void
{
    if (server::ServerInstance::getInstancePointer())
    {
        if (server::ServerInstance::getInstancePointer()->stop())
        {
            sendMessageTo(ctx, server_logger, logger::LogLevel::info, "Server stopped");
            return;
        }
    }
    sendMessageTo(ctx, server_logger, logger::LogLevel::error, "Failed to stop the server");
}
auto serverTerminate(CommandContext const &ctx) -> void
{
    if (server::ServerInstance::getInstancePointer())
    {
        if (server::ServerInstance::getInstancePointer()->kill())
        {
            sendMessageTo(ctx, server_logger, logger::LogLevel::info, "Server terminated");
            return;
        }
    }
    sendMessageTo(ctx, server_logger, logger::LogLevel::error, "Failed to terminate the server");
}
auto serverStatus(CommandContext const &ctx) -> void
{
}
} // namespace helium::commands