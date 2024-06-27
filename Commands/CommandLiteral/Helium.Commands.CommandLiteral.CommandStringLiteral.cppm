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
public:
    using CommandLiteralBase::CommandLiteralBase;

    auto tryAcceptToken(Token const &tok) const noexcept -> bool
    {
        if (tok.token_type != TokenCategory::TOKEN_PLAIN_STRING)
        {
            return false;
        }
        if (this->node_descriptor_->node_abbreviated_name.has_value())
        {
            if (tok.token_string == this->node_descriptor_->node_name or tok.token_string == this->node_descriptor_->node_abbreviated_name.value())
            {
                this->node_descriptor_->recent_accepted_token = tok;
                return true;
            }
        }
        if (tok.token_string == this->node_descriptor_->node_name)
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
};
} // namespace helium::commands