/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */
module;

#include <memory>
#include <optional>
#include <string>

#include <plf_hive.h>

#define FWD(x) ::std::forward<decltype(x)>(x)

export module Helium.Commands.CommandDispatcher;

import Helium.Base;
import Helium.Commands.Concepts;
import Helium.Commands.CommandBase;
import Helium.Commands.Lexer;

export namespace helium::commands
{
class CommandDispatcher final : public base::HeliumObject
{
private:
    plf::hive<std::shared_ptr<CommandNodeDescriptor>> command_roots_;
    CommandLexer lexer_;
    std::string recent_executed_command_;

public:
    template <typename Command>
    constexpr auto registerCommand(Command &&command) -> void
        requires concepts::IsCommandNode<Command>
    {
        command_roots_.insert(FWD(command).getNodeDescriptor());
    }

    auto tryExecuteCommand(std::string command)
    {
        this->recent_executed_command_ = std::move(command);
        if (auto opt = this->lexer_.processCommand(this->recent_executed_command_))
        {
            for (auto const &tok : opt.value())
            {
            }
        }
    }
};
} // namespace helium::commands