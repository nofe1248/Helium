module;

#include <iostream>
#include <string>

#include <replxx.hxx>

export module Helium.CLI.CLIMainLoop;

import Helium.Commands;
import Helium.Logger;

export namespace helium::cli
{

commands::CommandDispatcher dispatcher;
auto logger = logger::SharedLogger::getSharedLogger("CLI", "CLIMainLoop");

auto mainCLILoop()
{
    logger->info("Entering main CLI loop.");

    using namespace commands;

    // clang-format off
    dispatcher.registerCommand(
        CommandStringLiteral("#helium")
        .then(
            CommandStringLiteral("show")
            .then(
                CommandStringLiteral("warranty")
                .execute([](CommandContext const &ctx) -> void {
                    logger->trace("#helium show warranty command executed.");
                }),
                CommandStringLiteral("copyright")
                .execute([](CommandContext const &ctx) -> void {
                    logger->trace("#helium show copyright command executed.");
                })
            )
        )
    );
    // clang-format on

    std::string input_command;

    while (true)
    {
        std::getline(std::cin, input_command);
        if(not input_command.empty())
        {
            dispatcher.tryExecuteCommand(input_command);
        }
    }
}
} // namespace helium::cli