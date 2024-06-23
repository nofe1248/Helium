/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <optional>
#include <string>

export module Helium.Commands.CommandArgument.CommandArgumentBase;

import Helium.Commands.Concepts;
import Helium.Commands.CommandBase;

export namespace helium::commands
{
class CommandArgumentBase : public CommandNodeBase, public details::TagCommandArgument
{
protected:
    std::string recent_accepted_raw_value;

public:
    using CommandNodeBase::CommandNodeBase;
};
} // namespace helium::commands
