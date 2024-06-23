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

    // clang-format off

    using namespace commands;
    dispatcher.registerCommand(
        CommandStringLiteral("#helium")
        .then(
            CommandStringLiteral("show")
            .then(
                CommandStringLiteral("warranty"),
                CommandStringLiteral("copyright")
            )
        )
    );
    // clang-format on

    std::string input_command;

    while (true)
    {
        std::getline(std::cin, input_command);
    }
}
} // namespace helium::cli