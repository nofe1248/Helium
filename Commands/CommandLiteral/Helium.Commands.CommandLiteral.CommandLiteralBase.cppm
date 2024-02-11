/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <string>
#include <optional>

export module Helium.Commands.CommandLiteral.CommandLiteralBase;

import Helium.Commands.Concepts;
import Helium.Commands.CommandBase;

export namespace helium::commands {
    template<typename Derived_>
    class CommandLiteralBase : public CommandBase<CommandLiteralBase<Derived_>>, public details::TagCommandLiteral {
    public:
        using super = CommandBase<CommandLiteralBase<Derived_>>;

        constexpr CommandLiteralBase(CommandInfo info) : CommandBase<CommandLiteralBase>(info) {}
        constexpr CommandLiteralBase(std::string command_name, std::string command_help_message = "default",
                                      std::optional<std::string> command_abbreviated_name = std::nullopt) :
            CommandBase<CommandLiteralBase>(std::move(command_name), std::move(command_help_message),
                                             std::move(command_abbreviated_name)) {}
    };
} // namespace helium::commands
