/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <memory>
#include <string>
#include <optional>

#define FWD(x) ::std::forward<decltype(x)>(x)

export module Helium.Commands.CommandLiteral.CommandStringLiteral;

import Helium.Base;
import Helium.Commands.CommandBase;
import Helium.Commands.CommandLiteral.CommandLiteralBase;
import Helium.Commands.Concepts;
import Helium.Commands.CommandContext;

export namespace helium::commands {
	class CommandStringLiteral
		: public CommandLiteralBase<CommandStringLiteral>
    {
	public:
		using super = CommandLiteralBase<CommandStringLiteral>;

	    constexpr CommandStringLiteral(CommandInfo info) : CommandLiteralBase<CommandStringLiteral>(info) {
	        this->setProxy();
	    }
	    constexpr CommandStringLiteral(std::string command_name, std::string command_help_message = "default",
                                       std::optional<std::string> command_abbreviated_name = std::nullopt) :
                    CommandLiteralBase(std::move(command_name), std::move(command_help_message), std::move(command_abbreviated_name)) {
	        this->setProxy();
	    }
		auto tryAcceptCommand(std::string_view cmd) -> void {

	    }
	};
}