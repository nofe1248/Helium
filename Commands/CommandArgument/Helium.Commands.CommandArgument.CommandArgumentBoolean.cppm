/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <functional>
#include <string>
#include <memory>
#include <utility>

#define FWD(x) ::std::forward<decltype(x)>(x)

export module Helium.Commands.CommandArgument.CommandArgumentBoolean;

import Helium.Base;
import Helium.Commands.CommandBase;
import Helium.Commands.CommandArgument.CommandArgumentBase;
import Helium.Commands.CommandContext;
import Helium.Commands.Concepts;
import Helium.Commands.Lexer;

export namespace helium::commands
{
class CommandArgumentBoolean : public CommandArgumentBase
{
public:
    using CommandArgumentBase::CommandArgumentBase;
    using RawTokenStringConversionTarget = bool;

    static auto tryAcceptToken(std::shared_ptr<CommandNodeDescriptor> node_descriptor, Token const &tok) noexcept -> bool
    {
        if (tok.token_type == TokenCategory::TOKEN_BOOLEAN)
        {
            node_descriptor->recent_accepted_token = tok;
            return true;
        }
        return false;
    }
    static auto tokenSimilarity(std::shared_ptr<CommandNodeDescriptor> node_descriptor, Token const &tok) noexcept -> std::size_t
    {
        return 0;
    }
    static auto convertRawTokenToTargetType(std::shared_ptr<CommandNodeDescriptor> node_descriptor, Token const &tok) noexcept -> bool
    {
        if (tok.token_type == TokenCategory::TOKEN_BOOLEAN)
        {
            if (tok.token_string == "true")
            {
                return true;
            }
            if (tok.token_string == "false")
            {
                return false;
            }
        }
        std::unreachable();
    }
};
} // namespace helium::commands
