/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <string>

export module Helium.BuiltinCommands;

export import Helium.BuiltinCommands.Implementations;

import Helium.Commands.CommandDispatcher;
import Helium.Commands.CommandLiteral;
import Helium.Commands.CommandArgument;
import Helium.Commands.CommandContext;

export namespace helium::commands::builtins
{
struct BuiltinCommandRegisterHelper
{
    BuiltinCommandRegisterHelper()
    {
        // clang-format off
        using namespace commands;
        CommandDispatcher::getInstance().registerCommand(
            CommandStringLiteral("#helium")
            .then(
                CommandStringLiteral("exit")
                .execute(heliumExit),
                CommandStringLiteral("help")
                .execute(heliumHelp),
                CommandStringLiteral("about")
                .execute(heliumAbout),
                CommandStringLiteral("status")
                .execute(heliumStatus),
                CommandStringLiteral("server")
                .then(
                    CommandStringLiteral("start")
                    .execute(serverStart),
                    CommandStringLiteral("stop")
                    .execute(serverStop),
                    CommandStringLiteral("terminate")
                    .execute(serverTerminate),
                    CommandStringLiteral("status")
                    .execute(serverStatus)
                ),
                CommandStringLiteral("plugin")
                .then(
                    CommandStringLiteral("list")
                    .execute(pluginList),
                    CommandStringLiteral("load")
                    .then(
                        CommandArgumentQuotedString("plugin_name")
                        .execute(pluginLoad)
                    ),
                    CommandStringLiteral("unload")
                    .then(
                        CommandArgumentQuotedString("plugin_name")
                        .execute(pluginUnload)
                    ),
                    CommandStringLiteral("reload")
                    .then(
                        CommandArgumentQuotedString("plugin_name")
                        .execute(pluginReload)
                    ),
                    CommandStringLiteral("search")
                    .then(
                        CommandArgumentQuotedString("plugin_name")
                    .execute(pluginSearch)
                    ),
                    CommandStringLiteral("enable")
                    .then(
                        CommandArgumentQuotedString("plugin_name")
                        .execute(pluginEnable)
                    ),
                    CommandStringLiteral("disable")
                    .then(
                        CommandArgumentQuotedString("plugin_name")
                        .execute(pluginDisable)
                    ),
                    CommandStringLiteral("install")
                    .then(
                        CommandArgumentQuotedString("plugin_name")
                        .execute(pluginInstall)
                    ),
                    CommandStringLiteral("remove")
                    .then(
                        CommandArgumentQuotedString("plugin_name")
                        .execute(pluginRemove)
                    ),
                    CommandStringLiteral("status")
                    .then(
                        CommandArgumentQuotedString("plugin_name")
                        .execute(pluginStatus)
                    ),
                    CommandStringLiteral("source")
                    .then(
                        CommandStringLiteral("list")
                        .execute(pluginSourceList),
                        CommandStringLiteral("add")
                        .then(
                            CommandArgumentString("source_name")
                            .then(
                                CommandArgumentQuotedString("source_url")
                                .execute(pluginSourceAdd)
                            )
                        ),
                        CommandStringLiteral("remove")
                        .then(
                            CommandArgumentString("source_name")
                            .execute(pluginSourceRemove)
                        ),
                        CommandStringLiteral("update")
                        .then(
                            CommandArgumentString("source_name")
                            .execute(pluginSourceUpdate)
                        ),
                        CommandStringLiteral("update-all")
                        .execute(pluginSourceUpdateAll)
                    )
                ),
                CommandStringLiteral("debug")
                .then(
                    CommandArgumentBoolean("debug_enable"),
                    CommandStringLiteral("debug_logger")
                ),
                CommandStringLiteral("config")
                .then(
                    CommandStringLiteral("load"),
                    CommandStringLiteral("save")
                )
            )
        );
        // clang-format on
    }
} builtin_command_register_helper;
} // namespace helium::commands::builtins