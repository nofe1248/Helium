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
import Helium.Commands.Lexer;

export namespace helium::commands
{
class CommandArgumentString : public CommandArgumentBase
{
private:
    auto initCommandNode() -> void
    {
        this->node_descriptor_->try_accept_token = [this](Token const &token) -> bool {
            if (token.token_type == TokenCategory::TOKEN_QUOTED_STRING)
            {
                return true;
            }
            return false;
        };
    }

public:
    constexpr CommandArgumentString(std::string command_name, std::string command_description, std::optional<std::string> command_abbreviated_name = std::nullopt)
        : CommandArgumentBase(std::move(command_name), std::move(command_description), std::move(command_abbreviated_name))
    {
        this->initCommandNode();
    }
    constexpr CommandArgumentString(CommandNodeDescriptor info) : CommandArgumentBase(std::move(info))
    {
        this->initCommandNode();
    }
};
} // namespace helium::commands
