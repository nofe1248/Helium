/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <optional>
#include <string>

#include <proxy/proxy.h>

#define FWD(x) ::std::forward<decltype(x)>(x)

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
private:
    auto initCommandNode() -> void
    {
        this->node_descriptor_->try_accept_token = [this](Token const &token) -> bool {
            if (token.token_type == TokenCategory::TOKEN_INTEGER)
            {
                this->recent_accepted_raw_value = token.token_str;
                return true;
            }
            return false;
        };
    }

public:
    constexpr CommandArgumentInteger(std::string command_name, std::string command_description = "default_node_description", std::optional<std::string> command_abbreviated_name = std::nullopt)
        : CommandArgumentBase(std::move(command_name), std::move(command_description), std::move(command_abbreviated_name))
    {
        this->initCommandNode();
    }
    constexpr CommandArgumentInteger(CommandNodeDescriptor info) : CommandArgumentBase(std::move(info))
    {
        this->initCommandNode();
    }
};
} // namespace helium::commands
