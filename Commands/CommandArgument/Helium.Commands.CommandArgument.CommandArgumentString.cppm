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

        using CommandArgumentBase<CommandArgumentString>::CommandArgumentBase;
        auto tryAcceptCommand(std::string_view cmd) -> void {}
    };
} // namespace helium::commands
