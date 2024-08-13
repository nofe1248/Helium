/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <string>

export module Helium.BuiltinCommands;

export import Helium.BuiltinCommands.Implementations;

import Helium.Logger;
import Helium.Commands.CommandDispatcher;
import Helium.Commands.CommandLiteral;
import Helium.Commands.CommandArgument;
import Helium.Commands.CommandContext;
import Helium.Events;
import Helium.Server;
import Helium.Utils.RunLoopExecutor;

namespace helium::commands::builtins
{
auto logger = logger::SharedLogger::getSharedLogger("Commands", "Builtin");
}

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
                .execute([](CommandContext const &ctx) -> void {
                    events::EventEmitter emitter{events::EventBus::getInstancePointer()};
                    emitter.postponeEvent(events::HeliumStopping{});
                    server::ServerInstance::getInstancePointer()->stop();
                    utils::RunLoopExecutor::getInstance().finish();
                }),
                CommandStringLiteral("help"),
                CommandStringLiteral("about"),
                CommandStringLiteral("status"),
                CommandStringLiteral("test").then(CommandStringLiteral("opt").optional().then(CommandStringLiteral("opttest"))),
                CommandStringLiteral("server")
                .then(
                    CommandStringLiteral("start")
                    .execute([](CommandContext const &ctx) -> void {
                        if (server::ServerInstance::getInstancePointer())
                        {
                            if (not server::ServerInstance::getInstancePointer()->start())
                            {
                                logger->error("Failed to start server");
                            }
                        }
                    }),
                    CommandStringLiteral("stop")
                    .execute([](CommandContext const &ctx) -> void {
                        if (server::ServerInstance::getInstancePointer())
                        {
                            if (not server::ServerInstance::getInstancePointer()->stop())
                            {
                            logger->error("Failed to stop server");
                            }
                        }
                    }),
                    CommandStringLiteral("terminate")
                    .execute([](CommandContext const &ctx) -> void {
                        if (server::ServerInstance::getInstancePointer())
                        {
                            if (not server::ServerInstance::getInstancePointer()->kill())
                            {
                            logger->error("Failed to terminate server");
                            }
                        }
                    }),
                    CommandStringLiteral("status")
                ),
                CommandStringLiteral("plugin")
                .then(
                    CommandStringLiteral("list"),
                    CommandStringLiteral("load")
                    .then(
                        CommandArgumentQuotedString("plugin_name")
                    ),
                    CommandStringLiteral("unload")
                    .then(
                        CommandArgumentQuotedString("plugin_name")
                    ),
                    CommandStringLiteral("reload")
                    .then(
                        CommandArgumentQuotedString("plugin_name")
                    ),
                    CommandStringLiteral("search")
                    .then(
                        CommandArgumentQuotedString("plugin_name")
                    ),
                    CommandStringLiteral("enable")
                    .then(
                        CommandArgumentQuotedString("plugin_name")
                    ),
                    CommandStringLiteral("disable")
                    .then(
                        CommandArgumentQuotedString("plugin_name")
                    ),
                    CommandStringLiteral("install")
                    .then(
                        CommandArgumentQuotedString("plugin_name")
                    ),
                    CommandStringLiteral("remove")
                    .then(
                        CommandArgumentQuotedString("plugin_name")
                    ),
                    CommandStringLiteral("status")
                    .then(
                        CommandArgumentQuotedString("plugin_name")
                    ),
                    CommandStringLiteral("source")
                    .then(
                        CommandStringLiteral("list"),
                        CommandStringLiteral("add")
                        .then(
                            CommandArgumentString("source_name")
                            .then(
                                CommandArgumentQuotedString("source_url")
                            )
                        ),
                        CommandStringLiteral("remove")
                        .then(
                            CommandArgumentString("source_name")
                        ),
                        CommandStringLiteral("update")
                        .then(
                            CommandArgumentString("source_name")
                        ),
                        CommandStringLiteral("update-all")
                    )
                ),
                CommandStringLiteral("show")
                .then(
                    CommandStringLiteral("warranty")
                    .execute([](CommandContext const &ctx) -> void {
                        logger->debug("#helium show warranty command executed.");
                    }),
                    CommandStringLiteral("copyright")
                    .execute([](CommandContext const &ctx) -> void {
                        logger->debug("#helium show copyright command executed.");
                    })
                ),
                CommandStringLiteral("debug")
                .then(
                    CommandArgumentBoolean("debug_enable")
                    .execute([](CommandContext const &ctx, bool enabled) -> void {
                        if(enabled)
                        {
                            logger->debug("Debug mode enabled.");
                        }
                        else
                        {
                            logger->debug("Debug mode disabled.");
                        }
                    }),
                    CommandStringLiteral("debug_logger")
                    .then(
                        CommandArgumentString("logger_name")
                        .execute([](CommandContext const &ctx, std::string const &logger_name) -> void {
                            if (logger_name == "all") {

                            }
                        })
                    )
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