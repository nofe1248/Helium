/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <memory>
#include <optional>
#include <string>

#define FWD(x) ::std::forward<decltype(x)>(x)

export module Helium.Commands.CommandLiteral.CommandStringLiteral;

import Helium.Base;
import Helium.Commands.CommandBase;
import Helium.Commands.CommandLiteral.CommandLiteralBase;
import Helium.Commands.Concepts;
import Helium.Commands.CommandContext;
import Helium.Commands.Lexer;

export namespace helium::commands
{
class CommandStringLiteral : public CommandLiteralBase
{
private:
    auto initCommandNode() -> void
    {
        this->node_descriptor_->try_accept_token = [this](Token const &token) -> bool {
            if (token.token_type != TokenCategory::TOKEN_PLAIN_STRING)
            {
                return false;
            }
            if (this->node_descriptor_->node_abbreviated_name)
            {
                return token.token_str == this->node_descriptor_->node_name or token.token_str == this->node_descriptor_->node_abbreviated_name.value();
            }
            return token.token_str == this->node_descriptor_->node_name;
        };
    }

public:
    constexpr CommandStringLiteral(std::string command_name, std::string command_description, std::optional<std::string> command_abbreviated_name = std::nullopt)
        : CommandLiteralBase(std::move(command_name), std::move(command_description), std::move(command_abbreviated_name))
    {
        this->initCommandNode();
    }
    constexpr CommandStringLiteral(CommandNodeDescriptor info) : CommandLiteralBase(std::move(info))
    {
        this->initCommandNode();
    }
};
} // namespace helium::commands