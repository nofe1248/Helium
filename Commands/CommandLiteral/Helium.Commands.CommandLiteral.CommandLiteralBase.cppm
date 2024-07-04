/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <string>
#include <optional>

export module Helium.Commands.CommandLiteral.CommandLiteralBase;

import Helium.Commands.Concepts;
import Helium.Commands.CommandBase;

export namespace helium::commands {
class CommandLiteralBase : public CommandNodeBase, public details::TagCommandLiteral
{
public:
    using CommandNodeBase::CommandNodeBase;
};
} // namespace helium::commands
