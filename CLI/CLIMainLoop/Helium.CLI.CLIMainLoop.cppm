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
    /*
    dispatcher.registerCommand(
        CommandStringLiteral("#helium")
        .then(
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
            )
        )
    );*/
    int result = 0;
    auto calc = CommandStringLiteral("#calc");
    auto add = CommandStringLiteral("add");
    auto _ = add.then(
        CommandArgumentInteger<int>("add_number")
        .require([](CommandContext const &ctx, int num) -> bool {
            if (num < 0)
            {
                return false;
            }
            return true;
        })
        .execute([&result](CommandContext const &ctx, int num) -> void {
            result += num;
        })
        .redirect(calc)
    );
    _ = calc.then(
        CommandStringLiteral("print")
        .execute([&result](CommandContext const &ctx) -> void {
            logger->debug("Add result : {}", result);
            result = 0;
        })
        .redirect(calc)
    );
    _ = calc.then(add);
    dispatcher.registerCommand(
        calc
    );
    // clang-format on

    std::string input_command;
    CommandSource console_source{};

    while (true)
    {
        std::getline(std::cin, input_command);
        if (not input_command.empty())
        {
            bool execution_result = dispatcher.tryExecuteCommand(console_source, input_command);
        }
    }
}
} // namespace helium::cli