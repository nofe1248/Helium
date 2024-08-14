/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

export module Helium.BuiltinCommands.Implementations.Misc;

import Helium.Commands.CommandContext;
import Helium.Events;
import Helium.Server.ServerInstance;
import Helium.Utils.RunLoopExecutor;

export namespace helium::commands
{
auto heliumExit(CommandContext const &ctx) -> void
{
    events::EventEmitter emitter{events::EventBus::getInstancePointer()};
    emitter.postponeEvent(events::HeliumStopping{});
    server::ServerInstance::getInstancePointer()->stop();
    utils::RunLoopExecutor::getInstance().finish();
}
auto heliumHelp(CommandContext const &ctx) -> void
{

}
auto heliumAbout(CommandContext const &ctx) -> void
{

}
auto heliumStatus(CommandContext const &ctx) -> void
{

}
}