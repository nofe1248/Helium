/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <memory>
#include <string>
#include <utility>

export module Helium.Commands.CommandArgument.CommandArgumentFloatingPoint;

import Helium.Base;
import Helium.Commands.CommandBase;
import Helium.Commands.CommandArgument.CommandArgumentBase;
import Helium.Commands.CommandContext;
import Helium.Commands.Concepts;
import Helium.Commands.Lexer;

export namespace helium::commands
{
template <concepts::IsFloatingPoint FPType_>
class CommandArgumentFloatingPoint : public CommandArgumentBase
{
public:
    using CommandArgumentBase::CommandArgumentBase;

    using FloatingPointType = FPType_;
    using RawTokenStringConversionTarget = FloatingPointType;

    static auto tryAcceptToken(std::shared_ptr<CommandNodeDescriptor> const &node_descriptor, Token const &tok) noexcept -> TryAcceptTokenResult
    {
        if (tok.token_type == TokenCategory::TOKEN_FLOATING_POINT)
        {
            node_descriptor->recent_accepted_token = tok;
            return TryAcceptTokenResult{.accepted = true, .argument = CommandArgumentGeneric{convertRawTokenToTargetType(node_descriptor, tok)}};
        }
        return TryAcceptTokenResult{.accepted = false, .argument = std::nullopt};
    }
    static auto convertRawTokenToTargetType(std::shared_ptr<CommandNodeDescriptor> const &node_descriptor, Token const &tok) noexcept
        -> FloatingPointType
    {
        if (tok.token_type == TokenCategory::TOKEN_FLOATING_POINT)
        {
            FloatingPointType result;
            result = static_cast<FloatingPointType>(std::stold(tok.token_string));
            return result;
        }
        std::unreachable();
    }
};
} // namespace helium::commands
