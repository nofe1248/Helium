/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <memory>
#include <string>
#include <utility>

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
public:
    using CommandArgumentBase::CommandArgumentBase;

    using RawTokenStringConversionTarget = std::string;

    static auto tryAcceptToken(std::shared_ptr<CommandNodeDescriptor> const &node_descriptor, Token const &tok) noexcept -> bool
    {
        if (tok.token_type == TokenCategory::TOKEN_PLAIN_STRING)
        {
            node_descriptor->recent_accepted_token = tok;
            return true;
        }
        return false;
    }
    static auto convertRawTokenToTargetType(std::shared_ptr<CommandNodeDescriptor> const &node_descriptor, Token const &tok) noexcept -> std::string
    {
        if (tok.token_type == TokenCategory::TOKEN_PLAIN_STRING)
        {
            return tok.token_string;
        }
        std::unreachable();
    }
};
} // namespace helium::commands
