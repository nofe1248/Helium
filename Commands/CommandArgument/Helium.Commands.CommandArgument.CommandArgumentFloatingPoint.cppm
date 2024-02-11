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

export namespace helium::commands {
    template<concepts::IsFloatingPoint FPType_>
    struct FloatingPointMax {
        FPType_ max = std::numeric_limits<FPType_>::max();
    };

    template<concepts::IsFloatingPoint FPType_>
    struct FloatingPointMin {
        FPType_ min = std::numeric_limits<FPType_>::min();
    };

    template<concepts::IsFloatingPoint FPType_>
    class FloatingPointBound {
    public:
        using FloatingPointType = FPType_;
        using FloatingPointMaxType = FloatingPointMax<FloatingPointType>;
        using FloatingPointMinType = FloatingPointMin<FloatingPointType>;

    private:
        FloatingPointMaxType max_{};
        FloatingPointMinType min_{};

    public:
        constexpr explicit FloatingPointBound(FloatingPointMaxType max, FloatingPointMinType min) :
            max_(max), min_(min) {}
        constexpr explicit FloatingPointBound(FloatingPointMinType min) :
            FloatingPointBound(FloatingPointMaxType{}, min) {}
        constexpr explicit FloatingPointBound(FloatingPointMaxType max) :
            FloatingPointBound(max, FloatingPointMinType{}) {}
        constexpr explicit FloatingPointBound(FloatingPointMinType min, FloatingPointMaxType max) :
            FloatingPointBound(max, min) {}
        constexpr explicit FloatingPointBound() : FloatingPointBound(FloatingPointMaxType{}, FloatingPointMinType{}) {}
    };

    template<concepts::IsFloatingPoint FPType_>
    class CommandArgumentFloatingPoint : public CommandArgumentBase<CommandArgumentFloatingPoint<FPType_>> {
    public:
        using FloatingPointType = FPType_;
        using FloatingPointBoundType = FloatingPointBound<FloatingPointType>;
        using super = CommandArgumentBase<CommandArgumentFloatingPoint<FloatingPointType>>;

    private:
        FloatingPointBoundType bound_{};

    public:
        constexpr CommandArgumentFloatingPoint(CommandInfo info) : CommandArgumentBase<CommandArgumentFloatingPoint>(info) {
            this->setProxy();
        }
        constexpr CommandArgumentFloatingPoint(std::string command_name, std::string command_help_message = "default",
                                     std::optional<std::string> command_abbreviated_name = std::nullopt) :
            CommandArgumentBase<CommandArgumentFloatingPoint>(std::move(command_name), std::move(command_help_message),
                                                      std::move(command_abbreviated_name)) {
            this->setProxy();
        }
        auto tryAcceptCommand(std::string_view cmd) -> void {}
    };
} // namespace helium::commands
