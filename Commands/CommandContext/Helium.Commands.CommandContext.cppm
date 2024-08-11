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
    constexpr CommandContext(CommandSource source) : source_(std::move(source))
    {
    }

    constexpr auto getSource() const noexcept -> CommandSource const &
    {
        return this->source_;
    }
};
} // namespace helium::commands