/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

export module Helium.BuiltinCommands.Implementations.Server;

import Helium.Commands.CommandContext;
import Helium.Server.ServerInstance;
import Helium.Logger;

namespace helium::commands
{
auto server_logger = logger::SharedLogger::getSharedLogger("BuiltinCommands", "Server");
}

export namespace helium::commands
{
auto serverStart(CommandContext const &) -> void
{
    if (server::ServerInstance::getInstancePointer())
    {
        if (not server::ServerInstance::getInstancePointer()->start())
        {
            server_logger->error("Failed to start server");
        }
    }
}
auto serverStop(CommandContext const &) -> void
{
    if (server::ServerInstance::getInstancePointer())
    {
        if (not server::ServerInstance::getInstancePointer()->stop())
        {
            server_logger->error("Failed to stop server");
        }
    }
}
auto serverTerminate(CommandContext const &) -> void
{
    if (server::ServerInstance::getInstancePointer())
    {
        if (not server::ServerInstance::getInstancePointer()->kill())
        {
            server_logger->error("Failed to terminate server");
        }
    }
}
auto serverStatus(CommandContext const &) -> void
{

}
}