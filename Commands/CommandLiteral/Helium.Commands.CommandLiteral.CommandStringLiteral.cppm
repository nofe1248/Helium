/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
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

    static auto tryAcceptToken(std::shared_ptr<CommandNodeDescriptor> const &node_descriptor, Token const &tok) noexcept -> TryAcceptTokenResult
    {
        if (tok.token_type != TokenCategory::TOKEN_PLAIN_STRING)
        {
            return TryAcceptTokenResult{.accepted = false, .argument = std::nullopt};
        }
        if (node_descriptor->node_abbreviated_name.has_value())
        {
            if (tok.token_string == node_descriptor->node_name or tok.token_string == node_descriptor->node_abbreviated_name.value())
            {
                node_descriptor->recent_accepted_token = tok;
                return TryAcceptTokenResult{.accepted = true, .argument = std::nullopt};
            }
        }
        if (tok.token_string == node_descriptor->node_name)
        {
            node_descriptor->recent_accepted_token = tok;
            return TryAcceptTokenResult{.accepted = true, .argument = std::nullopt};
        }
        return TryAcceptTokenResult{.accepted = false, .argument = std::nullopt};
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