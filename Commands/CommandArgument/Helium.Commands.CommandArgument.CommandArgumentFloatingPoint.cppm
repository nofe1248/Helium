/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <functional>
#include <limits>
#include <memory>
#include <optional>
#include <string>

#include <proxy/proxy.h>

#define FWD(x) ::std::forward<decltype(x)>(x)

export module Helium.Commands.CommandArgument.CommandArgumentFloatingPoint;

import Helium.Base;
import Helium.Commands.CommandBase;
import Helium.Commands.CommandArgument.CommandArgumentBase;
import Helium.Commands.CommandContext;
import Helium.Commands.Concepts;
import Helium.Commands.Lexer;

export namespace helium::commands
{
template <concepts::IsFloatingPoint FPType_> class CommandArgumentFloatingPoint : public CommandArgumentBase
{
public:
    using CommandArgumentBase::CommandArgumentBase;

    using FloatingPointType = FPType_;

    auto tryAcceptToken(Token const& tok) const noexcept -> bool
    {
        if (tok.token_type == TokenCategory::TOKEN_FLOATING_POINT)
            {
                this->node_descriptor_->recent_accepted_token = tok;
                return true;
            }
            return false;
    }
    auto tokenSimilarity(Token const& tok) const noexcept -> std::size_t
    {
        return 0;
    }
};
} // namespace helium::commands
