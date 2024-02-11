/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <concepts>
#include <functional>
#include <memory>
#include <optional>
#include <string>

#include <proxy/proxy.h>

#define FWD(x) ::std::forward<decltype(x)>(x)

export module Helium.Commands.CommandArgument.CommandArgumentBoolean;

import Helium.Base;
import Helium.Commands.CommandBase;
import Helium.Commands.CommandArgument.CommandArgumentBase;
import Helium.Commands.CommandContext;
import Helium.Commands.Concepts;

export namespace helium::commands {
    class CommandArgumentBoolean : public CommandArgumentBase<CommandArgumentBoolean> {
    public:
        using super = CommandArgumentBase<CommandArgumentBoolean>;

        constexpr CommandArgumentBoolean(CommandInfo info) : CommandArgumentBase(info) { this->setProxy(); }
        constexpr CommandArgumentBoolean(std::string command_name, std::string command_help_message = "default",
                               std::optional<std::string> command_abbreviated_name = std::nullopt) :
            CommandArgumentBase(std::move(command_name), std::move(command_help_message), std::move(command_abbreviated_name)) {
            this->setProxy();
        }

        auto tryAcceptCommand(std::string_view cmd) -> void {}
    };
} // namespace helium::commands
