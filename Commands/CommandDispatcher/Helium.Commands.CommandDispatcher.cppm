/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */
module;

#include <algorithm>
#include <memory>
#include <optional>
#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

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
    plf::hive<Token> tokens_cache_;

public:
    template <typename Command>
    constexpr auto registerCommand(Command &&command) -> void
        requires concepts::IsCommandNode<Command>
    {
        this->command_root_.then(FWD(command));
    }

    auto tryExecuteCommand(CommandSource const &source, std::string const &command) -> bool
    {
        if (auto opt = this->lexer_.processCommand(command))
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
                        if (redirect_node->child_nodes.size() > 0)
                        {
                            for (auto child_node_of_redirected : redirect_node->child_nodes)
                            {
                                if (child_node_of_redirected->tryAcceptToken(*tok_it))
                                {
                                    current_node = child_node_of_redirected;
                                    matched = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if (not matched)
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
                if (current_node->child_nodes.size() > 0)
                {
                    for (auto child_node : current_node->child_nodes)
                    {
                        if (child_node->tryAcceptToken(*tok_it))
                        {
                            current_node = child_node;
                            matched = true;
                            break;
                        }
                    }
                }
                if (current_node->forward_nodes.has_value() and not matched)
                {
                    for (auto forward_node : current_node->forward_nodes.value())
                    {
                        if (forward_node->child_nodes.size() > 0)
                        {
                            for (auto child_node_of_forked : forward_node->child_nodes)
                            {
                                if (child_node_of_forked->tryAcceptToken(*tok_it))
                                {
                                    current_node = child_node_of_forked;
                                    matched = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if (not matched)
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
    }

    auto getSuggestions(std::string const &command, double similarity_cutoff) -> std::vector<std::string>
    {
        if (auto opt = this->lexer_.processCommand(command, false))
        {
            this->tokens_cache_ = std::move(opt.value());
        }
        else
        {
            return {};
        }

        auto current_node = this->command_root_.getNodeDescriptor().lock();
        auto tok_it = this->tokens_cache_.begin();
        std::vector<std::pair<std::string, double>> node_similarity;
        bool has_full_match = false;

        if (this->tokens_cache_.size() == 1)
        {
            if (current_node->is_redirected)
            {
                if (current_node->forward_nodes.has_value())
                {
                    for (auto redirect_node : current_node->forward_nodes.value())
                    {
                        if (redirect_node->child_nodes.size() > 0)
                        {
                            for (auto child_node_of_redirected : redirect_node->child_nodes)
                            {
                                if (child_node_of_redirected->auto_completable)
                                {
                                    if(double similarity = child_node_of_redirected->token_similarity(*tok_it); similarity >= similarity_cutoff)
                                    {
                                        if(similarity == 100.0f)
                                        {
                                            has_full_match = true;
                                        }
                                        node_similarity.push_back({child_node_of_redirected->node_name, similarity});
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                if (current_node->child_nodes.size() > 0)
                {
                    for (auto child_node : current_node->child_nodes)
                    {
                        if (child_node->auto_completable)
                        {
                            if(double similarity = child_node->token_similarity(*tok_it); similarity >= similarity_cutoff)
                            {
                                if(similarity == 100.0f)
                                {
                                    has_full_match = true;
                                }
                                node_similarity.push_back({child_node->node_name, similarity});
                            }
                        }
                    }
                }
                if (current_node->forward_nodes.has_value())
                {
                    for (auto forward_node : current_node->forward_nodes.value())
                    {
                        if (forward_node->child_nodes.size() > 0)
                        {
                            for (auto child_node_of_forked : forward_node->child_nodes)
                            {
                                if (child_node_of_forked->auto_completable)
                                {
                                    if(double similarity = child_node_of_forked->token_similarity(*tok_it); similarity >= similarity_cutoff)
                                    {
                                        if(similarity == 100.0f)
                                        {
                                            has_full_match = true;
                                        }
                                        node_similarity.push_back({child_node_of_forked->node_name, similarity});
                                    }
                                }
                            }
                        }
                    }
                }
            }
            std::ranges::sort(node_similarity, [](auto const &lhs, auto const &rhs) { return lhs.second < rhs.second; });
            return node_similarity | std::views::transform([](auto p) { return p.first; }) | std::ranges::to<std::vector<std::string>>();
        }

        for (; std::distance(tok_it, this->tokens_cache_.end()) >= 2; ++tok_it)
        {
            if (current_node->is_redirected)
            {
                bool matched = false;
                if (current_node->forward_nodes.has_value())
                {
                    for (auto redirect_node : current_node->forward_nodes.value())
                    {
                        if (redirect_node->child_nodes.size() > 0)
                        {
                            for (auto child_node_of_redirected : redirect_node->child_nodes)
                            {
                                if (child_node_of_redirected->tryAcceptToken(*tok_it))
                                {
                                    current_node = child_node_of_redirected;
                                    matched = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if (not matched)
                {
                    return {};
                }
            }
            else
            {
                bool matched = false;
                if (current_node->child_nodes.size() > 0)
                {
                    for (auto child_node : current_node->child_nodes)
                    {
                        if (child_node->tryAcceptToken(*tok_it))
                        {
                            current_node = child_node;
                            matched = true;
                            break;
                        }
                    }
                }
                if (current_node->forward_nodes.has_value() and not matched)
                {
                    for (auto forward_node : current_node->forward_nodes.value())
                    {
                        if (forward_node->child_nodes.size() > 0)
                        {
                            for (auto child_node_of_forked : forward_node->child_nodes)
                            {
                                if (child_node_of_forked->tryAcceptToken(*tok_it))
                                {
                                    current_node = child_node_of_forked;
                                    matched = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if (not matched)
                {
                    return {};
                }
            }
        }
        if (current_node->is_redirected)
        {
            if (current_node->forward_nodes.has_value())
            {
                for (auto redirect_node : current_node->forward_nodes.value())
                {
                    if (redirect_node->child_nodes.size() > 0)
                    {
                        for (auto child_node_of_redirected : redirect_node->child_nodes)
                        {
                            if (child_node_of_redirected->auto_completable)
                            {
                                if(double similarity = child_node_of_redirected->token_similarity(*tok_it); similarity >= similarity_cutoff)
                                {
                                    if(similarity == 100.0f)
                                    {
                                        has_full_match = true;
                                    }
                                    node_similarity.push_back({child_node_of_redirected->node_name, similarity});
                                }
                            }
                        }
                    }

                }
            }
        }
        else
        {
            if (current_node->child_nodes.size() > 0)
            {
                for (auto child_node : current_node->child_nodes)
                {
                    if (child_node->auto_completable)
                    {
                        if(double similarity = child_node->token_similarity(*tok_it); similarity >= similarity_cutoff)
                        {
                            if(similarity == 100.0f)
                            {
                                has_full_match = true;
                            }
                            node_similarity.push_back({child_node->node_name, similarity});
                        }
                    }
                }
            }
            if (current_node->forward_nodes.has_value())
            {
                for (auto forward_node : current_node->forward_nodes.value())
                {
                    if (forward_node->child_nodes.size() > 0)
                    {
                        for (auto child_node_of_forked : forward_node->child_nodes)
                        {
                            if (child_node_of_forked->auto_completable)
                            {
                                if(double similarity = child_node_of_forked->token_similarity(*tok_it); similarity >= similarity_cutoff)
                                {
                                    if(similarity == 100.0f)
                                    {
                                        has_full_match = true;
                                    }
                                    node_similarity.push_back({child_node_of_forked->node_name, similarity});
                                }
                            }
                        }
                    }
                }
            }
        }
        std::ranges::sort(node_similarity, [](auto const &lhs, auto const &rhs) { return lhs.second < rhs.second; });
        if(has_full_match)
        {
            return node_similarity | std::views::filter([](auto p) { return p.second == 100.0f; }) | std::views::transform([](auto p) { return p.first; }) | std::ranges::to<std::vector<std::string>>();
        }
        return node_similarity | std::views::transform([](auto p) { return p.first; }) | std::ranges::to<std::vector<std::string>>();
    }
};
} // namespace helium::commands