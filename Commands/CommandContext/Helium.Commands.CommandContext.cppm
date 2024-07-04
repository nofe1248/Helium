/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <utility>

export module Helium.Commands.CommandContext;

import Helium.Base;
import Helium.Commands.CommandSource;

export namespace helium::commands
{
class CommandContext : public base::HeliumObject
{
private:
    CommandSource source_;

public:
    constexpr CommandContext(CommandSource &&source) : source_(source)
    {
    }
    constexpr CommandContext(CommandSource const &source) : source_(source)
    {
    }

    constexpr auto getSource(this auto &&self) noexcept -> CommandSource const &
    {
        return std::forward<decltype(self)>(self).source_;
    }
};
} // namespace helium::commands