/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <optional>
#include <string>

export module Helium.Commands.CommandArgument.CommandArgumentBase;

import Helium.Commands.Concepts;
import Helium.Commands.CommandBase;

export namespace helium::commands {
    template<typename Derived_>
    class CommandArgumentBase : public CommandBase<CommandArgumentBase<Derived_>>, public details::TagCommandArgument {
    public:
        using super = CommandBase<CommandArgumentBase<Derived_>>;

        constexpr CommandArgumentBase(CommandInfo info) : CommandBase<CommandArgumentBase>(info) {}
        constexpr CommandArgumentBase(std::string command_name, std::string command_help_message = "default",
                                      std::optional<std::string> command_abbreviated_name = std::nullopt) :
            CommandBase<CommandArgumentBase>(std::move(command_name), std::move(command_help_message),
                                             std::move(command_abbreviated_name)) {}
    };
} // namespace helium::commands
