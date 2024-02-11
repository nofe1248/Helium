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

export namespace helium::commands {
    template<concepts::IsInteger IntegerType = std::int64_t>
    struct IntegerBoundMin {
        IntegerType value = std::numeric_limits<IntegerType>::min();
    };

    template<concepts::IsInteger IntegerType = std::int64_t>
    struct IntegerBoundMax {
        IntegerType value = std::numeric_limits<IntegerType>::max();
    };

    template<concepts::IsInteger IntegerType_ = std::int64_t>
    class IntegerBound {
    public:
        using IntegerType = IntegerType_;
        using IntegerBoundMaxType = IntegerBoundMax<IntegerType>;
        using IntegerBoundMinType = IntegerBoundMin<IntegerType>;

    private:
        IntegerBoundMaxType max_{};
        IntegerBoundMinType min_{};

    public:
        constexpr explicit IntegerBound(IntegerBoundMaxType max, IntegerBoundMinType min) : max_(max), min_(min) {}
        constexpr explicit IntegerBound(IntegerBoundMinType min) : IntegerBound(IntegerBoundMaxType{}, min) {}
        constexpr explicit IntegerBound(IntegerBoundMaxType max) : IntegerBound(max, IntegerBoundMinType{}) {}
        constexpr explicit IntegerBound(IntegerBoundMinType min, IntegerBoundMaxType max) : IntegerBound(max, min) {}
        constexpr explicit IntegerBound() : IntegerBound(IntegerBoundMaxType{}, IntegerBoundMinType{}) {}
    };

    template<concepts::IsInteger IntegerType_ = std::int64_t>
    class CommandArgumentInteger : public CommandArgumentBase<CommandArgumentInteger<IntegerType_>> {
    public:
        using IntegerType = IntegerType_;
        using IntegerBoundType = IntegerBound<IntegerType>;
        using super = CommandArgumentBase<CommandArgumentInteger<IntegerType>>;

    private:
        IntegerBoundType bound_{};

    public:
        constexpr CommandArgumentInteger(CommandInfo info) : CommandArgumentBase<CommandArgumentInteger>(info) { this->setProxy(); }
        constexpr CommandArgumentInteger(std::string command_name, std::string command_help_message = "default",
                               std::optional<std::string> command_abbreviated_name = std::nullopt) :
            CommandArgumentBase<CommandArgumentInteger>(std::move(command_name), std::move(command_help_message),
                                                std::move(command_abbreviated_name)) {
            this->setProxy();
        }
        auto tryAcceptCommand(std::string_view cmd) -> void {}
    };
} // namespace helium::commands
