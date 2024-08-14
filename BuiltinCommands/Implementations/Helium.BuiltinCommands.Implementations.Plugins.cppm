/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <string>

export module Helium.BuiltinCommands.Implementations.Plugins;

import Helium.Commands.CommandContext;
import Helium.Logger;

namespace helium::commands
{
auto plugin_logger = logger::SharedLogger::getSharedLogger("BuiltinCommands", "Plugin");
}

export namespace helium::commands
{
auto pluginList(CommandContext const &ctx) -> void
{
}
auto pluginLoad(CommandContext const &ctx, std::string const &plugin_name) -> void
{
}
auto pluginUnload(CommandContext const &ctx, std::string const &plugin_name) -> void
{
}
auto pluginReload(CommandContext const &ctx, std::string const &plugin_name) -> void
{
}
auto pluginSearch(CommandContext const &ctx, std::string const &plugin_name) -> void
{
}
auto pluginEnable(CommandContext const &ctx, std::string const &plugin_name) -> void
{
}
auto pluginDisable(CommandContext const &ctx, std::string const &plugin_name) -> void
{
}
auto pluginInstall(CommandContext const &ctx, std::string const &plugin_name) -> void
{
}
auto pluginRemove(CommandContext const &ctx, std::string const &plugin_name) -> void
{
}
auto pluginStatus(CommandContext const &ctx, std::string const &plugin_name) -> void
{
}
auto pluginSourceList(CommandContext const &ctx) -> void
{
}
auto pluginSourceAdd(CommandContext const &ctx, std::string const &source_url) -> void
{
    plugin_logger->debug("{}", ctx.getArgument("source_name").value().getString().value_or(""));
}
auto pluginSourceRemove(CommandContext const &ctx, std::string const &source_name) -> void
{
}
auto pluginSourceUpdate(CommandContext const &ctx, std::string const &source_name) -> void
{
}
auto pluginSourceUpdateAll(CommandContext const &ctx) -> void
{
}
} // namespace helium::commands