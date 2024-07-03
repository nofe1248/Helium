/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <concepts>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#define FWD(x) ::std::forward<decltype(x)>(x)

#include <plf_hive.h>

export module Helium.Commands.CommandBase;

import Helium.Base;
import Helium.Commands.CommandContext;
import Helium.Commands.Concepts;
import Helium.Commands.Lexer;

export namespace helium::commands
{
class CommandNodeDescriptor final
{
public:
    std::shared_ptr<CommandNodeDescriptor> parent_node = nullptr;
    plf::hive<std::shared_ptr<CommandNodeDescriptor>> child_nodes{};
    bool is_redirected = false;
    std::optional<plf::hive<std::shared_ptr<CommandNodeDescriptor>>> forward_nodes;

    std::string node_name = "default_node_name";
    std::string node_description = "default_node_description";
    std::optional<std::string> node_abbreviated_name = std::nullopt;

    Token recent_accepted_token = {TokenCategory::TOKEN_PLAIN_STRING, ""};

    std::optional<std::vector<std::function<bool(CommandContext const &, Token const &)>>> node_predicate = std::nullopt;
    std::optional<std::vector<std::function<void(CommandContext const &, Token const &)>>> node_callback = std::nullopt;

    std::function<bool(Token const &)> try_accept_token;
    std::function<std::size_t(Token const &)> token_similarity;

    CommandNodeDescriptor(std::string command_name, std::string command_description = "default_node_description", std::optional<std::string> command_abbreviated_name = std::nullopt)
        : node_name(std::move(command_name)), node_description(std::move(command_description)), node_abbreviated_name(std::move(command_abbreviated_name))
    {
    }

    constexpr CommandNodeDescriptor() = default;
    constexpr CommandNodeDescriptor(CommandNodeDescriptor const &) = default;
    constexpr CommandNodeDescriptor(CommandNodeDescriptor &&) noexcept = default;

    constexpr ~CommandNodeDescriptor() = default;

    constexpr auto operator=(CommandNodeDescriptor const &) -> CommandNodeDescriptor & = default;
    constexpr auto operator=(CommandNodeDescriptor &&) noexcept -> CommandNodeDescriptor & = default;

    auto tryAcceptTokenAndExecute(this auto &&self, CommandContext const &context, Token const &tok) noexcept -> bool
    {
        if (not self.try_accept_token and not self.token_similarity)
        {
            return false;
        }
        if (self.try_accept_token(tok))
        {
            if (self.is_redirected)
            {
            }
            else
            {
                bool predicate_result = true;
                if (self.node_predicate.has_value())
                {
                    for (auto const &predicate : self.node_predicate.value())
                    {
                        if (not predicate(context, tok))
                        {
                            predicate_result = false;
                            break;
                        }
                    }
                }
                if (self.forward_nodes.has_value())
                {
                }
                if (predicate_result)
                {
                }
            }
        }
        return false;
    }
};

class CommandNodeBase;
} // namespace helium::commands

export namespace helium::commands::concepts
{
template <typename Command_>
concept IsCommandNode =
    std::derived_from<Command_, commands::CommandNodeBase> and std::copyable<Command_> and std::movable<Command_> and requires(std::shared_ptr<CommandNodeDescriptor> descriptor, Token tok) {
        typename std::decay_t<Command_>::RawTokenStringConversionTarget;
        {
            std::decay_t<Command_>::tryAcceptToken(descriptor, tok)
        } -> std::same_as<bool>;
        {
            std::decay_t<Command_>::tokenSimilarity(descriptor, tok)
        } -> std::same_as<std::size_t>;
        {
            std::decay_t<Command_>::convertRawTokenToTargetType(descriptor, tok)
        } -> std::same_as<typename std::decay_t<Command_>::RawTokenStringConversionTarget>;
    } or requires(std::shared_ptr<CommandNodeDescriptor> descriptor, Token tok) {
        {
            std::decay_t<Command_>::tryAcceptToken(descriptor, tok)
        } -> std::same_as<bool>;
        {
            std::decay_t<Command_>::tokenSimilarity(descriptor, tok)
        } -> std::same_as<std::size_t>;
    };
} // namespace helium::commands::concepts

namespace helium::commands
{

class CommandNodeBase : public base::HeliumObject
{
protected:
    bool descriptor_initialized_ = false;
    std::shared_ptr<CommandNodeDescriptor> node_descriptor_ = std::make_shared<CommandNodeDescriptor>();

    template <std::invocable Pred_> constexpr auto addPredicate(this auto &&self, Pred_ &&pred) -> void
    {
        if (not FWD(self).node_descriptor_->node_predicate.has_value())
        {
            FWD(self).node_descriptor_->node_predicate = std::vector<std::function<bool()>>{};
        }
        FWD(self).node_descriptor_->node_predicate.push_back(FWD(pred));
    }

    template <std::invocable Callback_> constexpr auto addCallback(this auto &&self, Callback_ &&callback) -> void
    {
        if (not FWD(self).node_descriptor_->node_predicate.has_value())
        {
            FWD(self).node_descriptor_->node_predicate = std::vector<std::function<void(CommandContext const &, Token const &)>>{};
        }
        FWD(self).node_descriptor->node_callback.push_back(FWD(callback));
    }

    constexpr auto initializeNodeDescriptor(this auto &&self) -> void
    {
        using SelfT = std::decay_t<decltype(self)>;
        if (not self.descriptor_initialized_)
        {
            self.node_descriptor_->try_accept_token = [descriptor = self.node_descriptor_](Token const &tok) noexcept(noexcept(SelfT::tryAcceptToken(
                                                          std::declval<std::shared_ptr<CommandNodeDescriptor>>(), tok))) -> bool { return SelfT::tryAcceptToken(descriptor, tok); };
            self.node_descriptor_->token_similarity = [descriptor = self.node_descriptor_](Token const &tok) noexcept(noexcept(SelfT::tokenSimilarity(
                                                          std::declval<std::shared_ptr<CommandNodeDescriptor>>(), tok))) -> std::size_t { return SelfT::tokenSimilarity(descriptor, tok); };
            self.descriptor_initialized_ = true;
        }
    }

public:
    constexpr CommandNodeBase(std::string command_name, std::string command_description = "default_node_description", std::optional<std::string> command_abbreviated_name = std::nullopt)
        : node_descriptor_(std::make_shared<CommandNodeDescriptor>(std::move(command_name), std::move(command_description), std::move(command_abbreviated_name)))
    {
    }
    constexpr CommandNodeBase(CommandNodeDescriptor info)
    {
        *this->node_descriptor_ = std::move(info);
    }

    constexpr CommandNodeBase() = delete;
    constexpr CommandNodeBase(CommandNodeBase const &) = default;
    constexpr CommandNodeBase(CommandNodeBase &&) noexcept = default;

    constexpr auto operator=(CommandNodeBase const &) -> CommandNodeBase & = default;
    constexpr auto operator=(CommandNodeBase &&) noexcept -> CommandNodeBase & = default;

    [[nodiscard]] auto getNodeDescriptor(this auto &&self) -> std::weak_ptr<CommandNodeDescriptor>
        requires concepts::IsCommandNode<std::decay_t<decltype(self)>>
    {
        FWD(self).initializeNodeDescriptor();
        return FWD(self).node_descriptor_;
    }
    [[nodiscard]] auto getRecentAcceptedToken(this auto &&self) noexcept -> std::string
        requires concepts::IsCommandNode<std::decay_t<decltype(self)>>
    {
        return FWD(self).node_descriptor_->recent_accepted_token;
    }
    auto setParentNode(this auto &&self, std::shared_ptr<CommandNodeDescriptor> const parent) -> void
        requires concepts::IsCommandNode<std::decay_t<decltype(self)>>
    {
        FWD(self).node_descriptor_->parent_node = parent;
    }
    auto addChildNode(this auto &&self, std::shared_ptr<CommandNodeDescriptor> const child) -> void
        requires concepts::IsCommandNode<std::decay_t<decltype(self)>>
    {
        FWD(self).node_descriptor_->child_nodes.insert(child);
    }

    template <typename... Next_>
    [[nodiscard]] constexpr decltype(auto) then(this auto &&self, Next_ &&...next_node)
        requires concepts::IsCommandNode<std::decay_t<decltype(self)>> and (concepts::IsCommandNode<Next_> and ...)
    {
        (FWD(self).addChildNode(FWD(next_node).getNodeDescriptor().lock()), ...);
        (FWD(next_node).setParentNode(FWD(self).getNodeDescriptor().lock()), ...);
        return static_cast<std::decay_t<decltype(self)>>(std::move(FWD(self)));
    }

    template <typename... Callback_>
    [[nodiscard]] constexpr decltype(auto) execute(this auto &&self, Callback_ &&...callback)
        requires concepts::IsCommandNode<std::decay_t<decltype(self)>>
    {
        auto descriptor = FWD(self).getNodeDescriptor();
        FWD(self).addCallback([descriptor, callback...](CommandContext const &context, Token const &tok) {
            using SelfT = std::decay_t<decltype(self)>;
            if constexpr (requires { typename SelfT::RawTokenStringConversionTarget; })
            {
                typename SelfT::RawTokenStringConversionTarget converted = SelfT::convertRawTokenToTargetType(descriptor, tok);
                (callback(context, converted), ...);
            }
            else
            {
                (callback(context), ...);
            }
        });
        return static_cast<std::decay_t<decltype(self)>>(std::move(FWD(self)));
    }

    template <typename... Pred_>
    [[nodiscard]] constexpr decltype(auto) require(this auto &&self, Pred_ &&...pred)
        requires concepts::IsCommandNode<std::decay_t<decltype(self)>>
    {
        auto descriptor = FWD(self).getNodeDescriptor();
        FWD(self).addPredicate([descriptor, pred...](CommandContext const &context, Token const &tok) -> bool {
            using SelfT = std::decay_t<decltype(self)>;
            if constexpr (requires { typename SelfT::RawTokenStringConversionTarget; })
            {
                typename SelfT::RawTokenStringConversionTarget converted = SelfT::convertRawTokenToTargetType(descriptor, tok);
                return (pred(context, converted) and ...);
            }
            else
            {
                return (pred(context) and ...);
            }
        });
        return static_cast<std::decay_t<decltype(self)>>(std::move(FWD(self)));
    }

    template <concepts::IsCommandNode... Fork_>
    [[nodiscard]] constexpr decltype(auto) fork(this auto &&self, Fork_ &&...fork)
        requires concepts::IsCommandNode<std::decay_t<decltype(self)>>
    {
        return static_cast<std::decay_t<decltype(self)>>(std::move(FWD(self)));
    }

    template <concepts::IsCommandNode... Redirect_>
    [[nodiscard]] constexpr decltype(auto) redirect(this auto &&self, Redirect_ &&...redirect)
        requires concepts::IsCommandNode<std::decay_t<decltype(self)>>
    {
        return static_cast<std::decay_t<decltype(self)>>(std::move(FWD(self)));
    }
};
} // namespace helium::commands
