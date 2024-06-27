/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <functional>
#include <string>

#include <proxy/proxy.h>

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

    auto tryAcceptToken(Token const &tok) noexcept -> bool
    {
        if (tok.token_type == TokenCategory::TOKEN_BOOLEAN)
        {
            this->node_descriptor_->recent_accepted_token = tok;
            return true;
        }
        return false;
    }
    auto tokenSimilarity(Token const &tok) const noexcept -> std::size_t
    {
        return 0;
    }
    auto convertRawTokenToTargetType(Token const &tok) const noexcept -> bool
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
