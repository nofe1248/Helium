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
import Helium.Commands.CommandSource;
import Helium.Commands.CommandContext;
import Helium.Commands.CommandRoot;
import Helium.Commands.Lexer;
import Helium.Logger.SharedLogger;

namespace helium::commands
{
auto dispatcher_logger = logger::SharedLogger::getSharedLogger("Commands", "CommandDispatcher");
}

export namespace helium::commands
{
class CommandDispatcher final : public base::HeliumObject
{
private:
    CommandRoot command_root_{};
    CommandLexer lexer_;
    std::string recent_executed_command_;
    plf::hive<Token> tokens_cache_;

public:
    template <typename Command>
    constexpr auto registerCommand(Command &&command) -> void
        requires concepts::IsCommandNode<Command>
    {
        this->command_root_.then(FWD(command));
    }

    auto tryExecuteCommand(CommandSource const &source, std::string command) -> bool
    {
        this->recent_executed_command_ = std::move(command);
        if (auto opt = this->lexer_.processCommand(this->recent_executed_command_))
        {
            this->tokens_cache_ = std::move(opt.value());
        }
        else
        {
            return false;
        }
        auto current_node = this->command_root_.getNodeDescriptor().lock();
        CommandContext context{source};
        for (auto tok_it = this->tokens_cache_.begin(); tok_it != this->tokens_cache_.end(); ++tok_it)
        {
            if (current_node->is_redirected)
            {
                bool matched = false;
                if (current_node->forward_nodes.has_value())
                {
                    for (auto redirect_node : current_node->forward_nodes.value())
                    {
                        if(redirect_node->child_nodes.size() > 0)
                        {
                            for(auto child_node_of_redirected : redirect_node->child_nodes)
                            {
                                if (child_node_of_redirected->tryAcceptToken(context, *tok_it))
                                {
                                    current_node = child_node_of_redirected;
                                    matched = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if(not matched)
                {
                    return false;
                }
                if (not current_node->executeCallbacks(context, *tok_it))
                {
                    return false;
                }
            }
            else
            {
                bool matched = false;
                if(current_node->child_nodes.size() > 0)
                {
                    for(auto child_node : current_node->child_nodes)
                    {
                        if (child_node->tryAcceptToken(context, *tok_it))
                        {
                            current_node = child_node;
                            matched = true;
                            break;
                        }
                    }
                }
                if(current_node->forward_nodes.has_value() and not matched)
                {
                    for (auto forward_node : current_node->forward_nodes.value())
                    {
                        if(forward_node->child_nodes.size() > 0)
                        {
                            for(auto child_node_of_forked : forward_node->child_nodes)
                            {
                                if (child_node_of_forked->tryAcceptToken(context, *tok_it))
                                {
                                    current_node = child_node_of_forked;
                                    matched = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if(not matched)
                {
                    return false;
                }
                if (not current_node->executeCallbacks(context, *tok_it))
                {
                    return false;
                }
            }
        }
        if (current_node->child_nodes.size() == 0)
        {
            return true;
        }
        return false;
        return false;
    }
};
} // namespace helium::commands