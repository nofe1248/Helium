/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <functional>
#include <memory>
#include <optional>
#include <string>

#define FWD(x) ::std::forward<decltype(x)>(x)

export module Helium.Commands.CommandArgument.CommandArgumentString;

import Helium.Base;
import Helium.Commands.CommandBase;
import Helium.Commands.CommandArgument.CommandArgumentBase;
import Helium.Commands.CommandContext;
import Helium.Commands.Concepts;

export namespace helium::commands {
    template<concepts::IsString StrType_>
    class CommandArgumentString : public CommandArgumentBase<CommandArgumentString<StrType_>> {
    public:
        using StringType = StrType_;
        using super = CommandArgumentBase<CommandArgumentString>;

        constexpr CommandArgumentString(CommandInfo info) : CommandArgumentBase<CommandArgumentString>(info) { this->setProxy(); }
        constexpr CommandArgumentString(std::string command_name, std::string command_help_message = "default",
                              std::optional<std::string> command_abbreviated_name = std::nullopt) :
            CommandArgumentBase<CommandArgumentString>(std::move(command_name), std::move(command_help_message),
                                               std::move(command_abbreviated_name)) {
            this->setProxy();
        }
        auto tryAcceptCommand(std::string_view cmd) -> void {}
    };
} // namespace helium::commands
