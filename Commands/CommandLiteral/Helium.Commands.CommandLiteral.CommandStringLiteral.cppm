/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <memory>
#include <optional>
#include <string>

#include <rapidfuzz/fuzz.hpp>

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

    static auto tryAcceptToken(std::shared_ptr<CommandNodeDescriptor> const &node_descriptor, Token const &tok) noexcept -> bool
    {
        if (tok.token_type != TokenCategory::TOKEN_PLAIN_STRING)
        {
            return false;
        }
        if (node_descriptor->node_abbreviated_name.has_value())
        {
            if (tok.token_string == node_descriptor->node_name or tok.token_string == node_descriptor->node_abbreviated_name.value())
            {
                node_descriptor->recent_accepted_token = tok;
                return true;
            }
        }
        if (tok.token_string == node_descriptor->node_name)
        {
            node_descriptor->recent_accepted_token = tok;
            return true;
        }
        return false;
    }
    static auto tokenSimilarity(std::shared_ptr<CommandNodeDescriptor> const &node_descriptor, Token const &tok) noexcept -> double
    {
        return node_descriptor->node_abbreviated_name.has_value() ? std::max(rapidfuzz::fuzz::partial_ratio(tok.token_string, node_descriptor->node_abbreviated_name.value()),
                                                                             rapidfuzz::fuzz::partial_ratio(tok.token_string, node_descriptor->node_name))
                                                                  : rapidfuzz::fuzz::partial_ratio(tok.token_string, node_descriptor->node_name);
    }
    static auto getSuggestion(std::shared_ptr<CommandNodeDescriptor> const &node_descriptor, Token const &tok) noexcept -> std::string
    {
        return node_descriptor->node_name;
    }
};
} // namespace helium::commands