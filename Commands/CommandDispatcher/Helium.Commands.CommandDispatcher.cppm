/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
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
    static auto getInstance() noexcept -> CommandDispatcher &
    {
        static CommandDispatcher instance;
        return instance;
    }

    template <typename Command>
    constexpr auto registerCommand(Command &&command) -> void
        requires concepts::IsCommandNode<Command>
    {
        (void)this->command_root_.then(FWD(command));
    }

    constexpr auto registerRawCommandNodeDescriptor(std::shared_ptr<CommandNodeDescriptor> descriptor) -> void
    {
        this->command_root_.addChildNode(descriptor);
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
        std::vector<std::shared_ptr<CommandNodeDescriptor>> all_forwarded_nodes{}, nodes_to_be_processed{};
        for (auto tok_it = this->tokens_cache_.begin(); tok_it != this->tokens_cache_.end(); ++tok_it)
        {
            auto find_all_forwarded = [&all_forwarded_nodes](this auto &&self, std::shared_ptr<CommandNodeDescriptor> const &node) -> void {
                if (not node->is_redirected)
                {
                    all_forwarded_nodes.push_back(node);
                }
                if (node->forward_nodes.has_value())
                {
                    for (auto const &redirect_node : node->forward_nodes.value())
                    {
                        self(redirect_node);
                    }
                }
            };

            if (not current_node->is_redirected)
            {
                all_forwarded_nodes.push_back(current_node);
            }
            if (current_node->forward_nodes.has_value())
            {
                for (auto const &forward_node : current_node->forward_nodes.value())
                {
                    find_all_forwarded(forward_node);
                }
            }

            for (auto const &forward_node : all_forwarded_nodes)
            {
                for (auto const &child_node : forward_node->child_nodes)
                {
                    nodes_to_be_processed.push_back(child_node);
                }
            }

            auto try_to_match_optional =
                [&tok_it, &context](this auto &&self,
                                    std::shared_ptr<CommandNodeDescriptor> const &node) -> std::optional<std::shared_ptr<CommandNodeDescriptor>> {
                if (auto const opt = node->tryAcceptToken(*tok_it); opt.has_value() and opt.value().accepted)
                {
                    if (opt.value().argument.has_value())
                    {
                        context.addArgument(node->node_name, opt.value().argument.value());
                    }
                    if (node->executeCallbacks(context, *tok_it))
                    {
                        return node;
                    }
                    return std::nullopt;
                }

                if (node->is_optional)
                {
                    std::vector<std::shared_ptr<CommandNodeDescriptor>> opt_forwarded_nodes{}, opt_nodes_to_be_processed{};

                    auto find_all_forwarded_opt = [&opt_forwarded_nodes](this auto &&self,
                                                                         std::shared_ptr<CommandNodeDescriptor> const &node) -> void {
                        if (not node->is_redirected)
                        {
                            opt_forwarded_nodes.push_back(node);
                        }
                        if (node->forward_nodes.has_value())
                        {
                            for (auto const &redirect_node : node->forward_nodes.value())
                            {
                                self(redirect_node);
                            }
                        }
                    };

                    if (not node->is_redirected)
                    {
                        opt_forwarded_nodes.push_back(node);
                    }
                    if (node->forward_nodes.has_value())
                    {
                        for (auto const &forward_node : node->forward_nodes.value())
                        {
                            find_all_forwarded_opt(forward_node);
                        }
                    }

                    for (auto const &forward_node : opt_forwarded_nodes)
                    {
                        for (auto const &child_node : forward_node->child_nodes)
                        {
                            opt_nodes_to_be_processed.push_back(child_node);
                        }
                    }

                    for (auto const &process_node : opt_nodes_to_be_processed)
                    {
                        if (auto opt = self(process_node))
                        {
                            return opt;
                        }
                    }
                }

                return std::nullopt;
            };

            for (auto const &node : nodes_to_be_processed)
            {
                if (node->is_optional)
                {
                    if (auto node_opt = try_to_match_optional(node))
                    {
                        current_node = node_opt.value();
                        break;
                    }
                    return false;
                }
                if (auto const opt = node->tryAcceptToken(*tok_it); opt.has_value() and opt.value().accepted)
                {
                    if (opt.value().argument.has_value())
                    {
                        context.addArgument(node->node_name, opt.value().argument.value());
                    }
                    if (node->executeCallbacks(context, *tok_it))
                    {
                        current_node = node;
                        break;
                    }
                    return false;
                }
            }

            all_forwarded_nodes.clear();
            nodes_to_be_processed.clear();
        }
        if (current_node->child_nodes.size() == 0)
        {
            return true;
        }
        return false;
    }

    auto getSuggestions(std::string const &command, double similarity_cutoff, bool show_all_result = false) -> std::vector<std::string>
    {
        if (auto opt = this->lexer_.processCommand(command, false))
        {
            this->tokens_cache_ = std::move(opt.value());
        }
        else
        {
            return {};
        }

        if (show_all_result)
        {
            this->tokens_cache_.insert(Token{TokenCategory::TOKEN_PLAIN_STRING, ""});
        }

        auto current_node = this->command_root_.getNodeDescriptor().lock();
        auto tok_it = this->tokens_cache_.begin();
        std::vector<std::pair<std::string, double>> node_similarity;
        bool has_full_match = false;
        std::vector<std::shared_ptr<CommandNodeDescriptor>> all_forwarded_nodes{}, nodes_to_be_processed{};
        auto find_all_forwarded = [&all_forwarded_nodes](this auto &&self, std::shared_ptr<CommandNodeDescriptor> const &node) -> void {
            if (not node->is_redirected)
            {
                all_forwarded_nodes.push_back(node);
            }
            if (node->forward_nodes.has_value())
            {
                for (auto const &redirect_node : node->forward_nodes.value())
                {
                    self(redirect_node);
                }
            }
        };

        for (; std::distance(tok_it, this->tokens_cache_.end()) >= 2; ++tok_it)
        {
            if (not current_node->is_redirected)
            {
                all_forwarded_nodes.push_back(current_node);
            }
            if (current_node->forward_nodes.has_value())
            {
                for (auto const &forward_node : current_node->forward_nodes.value())
                {
                    find_all_forwarded(forward_node);
                }
            }

            for (auto const &forward_node : all_forwarded_nodes)
            {
                for (auto const &child_node : forward_node->child_nodes)
                {
                    nodes_to_be_processed.push_back(child_node);
                }
            }

            auto try_to_match_optional =
                [&tok_it](this auto &&self,
                          std::shared_ptr<CommandNodeDescriptor> const &node) -> std::optional<std::shared_ptr<CommandNodeDescriptor>> {
                if (auto const opt = node->tryAcceptToken(*tok_it); opt.has_value() and opt.value().accepted)
                {
                    return node;
                }

                if (node->is_optional)
                {
                    std::vector<std::shared_ptr<CommandNodeDescriptor>> opt_forwarded_nodes{}, opt_nodes_to_be_processed{};

                    auto find_all_forwarded_opt = [&opt_forwarded_nodes](this auto &&self,
                                                                         std::shared_ptr<CommandNodeDescriptor> const &node) -> void {
                        if (not node->is_redirected)
                        {
                            opt_forwarded_nodes.push_back(node);
                        }
                        if (node->forward_nodes.has_value())
                        {
                            for (auto const &redirect_node : node->forward_nodes.value())
                            {
                                self(redirect_node);
                            }
                        }
                    };

                    if (not node->is_redirected)
                    {
                        opt_forwarded_nodes.push_back(node);
                    }
                    if (node->forward_nodes.has_value())
                    {
                        for (auto const &forward_node : node->forward_nodes.value())
                        {
                            find_all_forwarded_opt(forward_node);
                        }
                    }

                    for (auto const &forward_node : opt_forwarded_nodes)
                    {
                        for (auto const &child_node : forward_node->child_nodes)
                        {
                            opt_nodes_to_be_processed.push_back(child_node);
                        }
                    }

                    for (auto const &process_node : opt_nodes_to_be_processed)
                    {
                        if (auto opt = self(process_node))
                        {
                            return opt;
                        }
                    }
                }

                return std::nullopt;
            };

            for (auto const &node : nodes_to_be_processed)
            {
                if (node->is_optional)
                {
                    if (auto node_opt = try_to_match_optional(node))
                    {
                        current_node = node_opt.value();
                        break;
                    }
                }
                if (auto const opt = node->tryAcceptToken(*tok_it); opt.has_value() and opt.value().accepted)
                {
                    current_node = node;
                    break;
                }
            }

            all_forwarded_nodes.clear();
            nodes_to_be_processed.clear();
        }

        all_forwarded_nodes.clear();
        nodes_to_be_processed.clear();

        if (not current_node->is_redirected)
        {
            all_forwarded_nodes.push_back(current_node);
        }
        if (current_node->forward_nodes.has_value())
        {
            for (auto const &forward_node : current_node->forward_nodes.value())
            {
                find_all_forwarded(forward_node);
            }
        }

        for (auto const &forward_node : all_forwarded_nodes)
        {
            for (auto const &child_node : forward_node->child_nodes)
            {
                nodes_to_be_processed.push_back(child_node);
            }
        }

        for (auto const &node : nodes_to_be_processed)
        {
            if (node->auto_completable)
            {
                if (double similarity = node->token_similarity(*tok_it); similarity >= similarity_cutoff or show_all_result)
                {
                    if (similarity == 100.0f)
                    {
                        has_full_match = true;
                    }
                    node_similarity.push_back({node->node_name, similarity});
                }
            }
        }

        std::ranges::sort(node_similarity, [](auto const &lhs, auto const &rhs) { return lhs.second < rhs.second; });
        if (has_full_match and not show_all_result)
        {
            return node_similarity | std::views::filter([](auto p) { return p.second == 100.0f; }) |
                   std::views::transform([](auto p) { return p.first; }) | std::ranges::to<std::vector<std::string>>();
        }
        return node_similarity | std::views::transform([](auto p) { return p.first; }) | std::ranges::to<std::vector<std::string>>();
    }
};
} // namespace helium::commands