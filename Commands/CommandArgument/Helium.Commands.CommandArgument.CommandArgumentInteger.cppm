/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <memory>
#include <charconv>
#include <string>
#include <utility>

export module Helium.Commands.CommandArgument.CommandArgumentInteger;

import Helium.Base;
import Helium.Commands.Concepts;
import Helium.Commands.CommandArgument.CommandArgumentBase;
import Helium.Commands.CommandContext;
import Helium.Commands.CommandBase;
import Helium.Commands.Lexer;

export namespace helium::commands
{
template <concepts::IsInteger IntegerType_ = std::int64_t> class CommandArgumentInteger : public CommandArgumentBase
{
public:
    using CommandArgumentBase::CommandArgumentBase;

    using IntegerType = IntegerType_;
    using RawTokenStringConversionTarget = IntegerType;

    static auto tryAcceptToken(std::shared_ptr<CommandNodeDescriptor> const &node_descriptor, Token const &tok) noexcept -> bool
    {
        if (tok.token_type == TokenCategory::TOKEN_INTEGER)
        {
            node_descriptor->recent_accepted_token = tok;
            return true;
        }
        return false;
    }
    static auto convertRawTokenToTargetType(std::shared_ptr<CommandNodeDescriptor> const &node_descriptor, Token const &tok) noexcept -> IntegerType
    {
        if (tok.token_type == TokenCategory::TOKEN_INTEGER)
        {
            IntegerType result;
            auto [ptr, ec] = std::from_chars(tok.token_string.data(), tok.token_string.data() + tok.token_string.size(), result);
            if(ec == std::errc())
            {
                return result;
            }
        }
        std::unreachable();
    }
};
} // namespace helium::commands
