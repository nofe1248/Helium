/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <concepts>
#include <memory>
#include <optional>
#include <string>
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
struct CommandNodeDescriptor
{
    std::shared_ptr<CommandNodeDescriptor> parent_node;
    plf::hive<std::shared_ptr<CommandNodeDescriptor>> child_nodes;

    std::string node_name = "default_node_name";
    std::string node_description = "default_node_description";
    std::optional<std::string> node_abbreviated_name = std::nullopt;

    Token recent_accepted_token = {TokenCategory::TOKEN_PLAIN_STRING, ""};

    std::optional<std::function<bool()>> node_predicate = std::nullopt;
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

    constexpr auto operator=(CommandNodeDescriptor const &) -> CommandNodeDescriptor & = default;
    constexpr auto operator=(CommandNodeDescriptor &&) noexcept -> CommandNodeDescriptor & = default;
};

class CommandNodeBase;
} // namespace helium::commands

export namespace helium::commands::concepts
{
template <typename Command_>
concept IsCommandNode = std::derived_from<Command_, commands::CommandNodeBase> and std::copyable<Command_> and std::movable<Command_> and requires(Command_ cmd, Token tok) {
    typename Command_::RawTokenStringConversionTarget;
    {
        cmd.tryAcceptToken(tok)
    } -> std::same_as<bool>;
    {
        cmd.tokenSimilarity(tok)
    } -> std::same_as<std::size_t>;
    {
        cmd.convertRawTokenToTargetType(tok)
    } -> std::same_as<typename Command_::RawTokenStringConversionTarget>;
} or requires(Command_ cmd, Token tok) {
    {
        cmd.tryAcceptToken(tok)
    } -> std::same_as<bool>;
    {
        cmd.tokenSimilarity(tok)
    } -> std::same_as<std::size_t>;
};
} // namespace helium::commands::concepts

namespace helium::commands
{

class CommandNodeBase : public base::HeliumObject
{
protected:
    bool descriptor_initialized_ = false;
    std::shared_ptr<CommandNodeDescriptor> node_descriptor_;

    template <std::invocable Pred_> constexpr auto addPredicate(this auto &&self, Pred_ &&pred) -> void
    {
        FWD(self).node_descriptor_->node_predicaate.insert(FWD(pred));
    }

    template <std::invocable<CommandContext const &> Callback_> constexpr auto addCallback(this auto &&self, Callback_ &&callback) -> void
    {
        FWD(self).node_descriptor->parent_node = FWD(callback);
    }

public:
    constexpr CommandNodeBase(std::string command_name, std::string command_description = "default_node_description", std::optional<std::string> command_abbreviated_name = std::nullopt)
        : node_descriptor_(std::make_shared<CommandNodeDescriptor>())
    {
        this->node_descriptor_->node_name = std::move(command_name);
        this->node_descriptor_->node_description = std::move(command_description);
        this->node_descriptor_->node_abbreviated_name = std::move(command_abbreviated_name);
    }
    constexpr CommandNodeBase(CommandNodeDescriptor info) : node_descriptor_(std::make_shared<CommandNodeDescriptor>())
    {
        *this->node_descriptor_ = std::move(info);
    }

    constexpr CommandNodeBase() = delete;
    constexpr CommandNodeBase(CommandNodeBase const &) = default;
    constexpr CommandNodeBase(CommandNodeBase &&) noexcept = default;

    constexpr auto operator=(CommandNodeBase const &) -> CommandNodeBase & = default;
    constexpr auto operator=(CommandNodeBase &&) noexcept -> CommandNodeBase & = default;

    [[nodiscard]] auto getNodeDescriptor(this auto &&self) -> std::shared_ptr<CommandNodeDescriptor>
        requires concepts::IsCommandNode<std::decay_t<decltype(self)>>
    {
        if (not self.descriptor_initialized_)
        {
            self.node_descriptor_->try_accept_token = [self](Token const &tok) noexcept(noexcept(self.tryAcceptToken(tok))) -> bool { return self.tryAcceptToken(tok); };
            self.node_descriptor_->token_similarity = [self](Token const &tok) noexcept(noexcept(self.tokenSimilarity(tok))) -> std::size_t { return self.tokenSimilarity(tok); };
            self.descriptor_initialized_ = true;
        }
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
        (FWD(next_node).setParentNode(FWD(self).getNodeDescriptor()), ...);
        (FWD(self).addChildNode(FWD(next_node).getNodeDescriptor()), ...);
        return static_cast<std::decay_t<decltype(self)>>(std::move(FWD(self)));
    }

    template <std::invocable... Callback_>
    [[nodiscard]] constexpr decltype(auto) execute(this auto &&self, Callback_ &&...callback)
        requires concepts::IsCommandNode<std::decay_t<decltype(self)>>
    {
        FWD(self).addCallback([self, callback...](CommandContext const &context, Token const &tok) {
            using SelfT = std::decay_t<decltype(self)>;
            if constexpr (requires { typename SelfT::RawTokenStringConversionTarget; })
            {
                typename SelfT::RawTokenStringConversionTarget converted = self.convertRawTokenToTargetType(tok);
                (callback(context, converted), ...);
            }
            else
            {
                (callback(context), ...);
            }
        });
        return static_cast<std::decay_t<decltype(self)>>(std::move(FWD(self)));
    }

    template <std::invocable... Pred_>
    [[nodiscard]] constexpr decltype(auto) require(this auto &&self, Pred_ &&...pred)
        requires concepts::IsCommandNode<std::decay_t<decltype(self)>>
    {
        (FWD(self).addPredicate(FWD(pred)), ...);
        return static_cast<std::decay_t<decltype(self)>>(std::move(FWD(self)));
    }

    template <concepts::IsCommandNode Redirect_>
    [[nodiscard]] constexpr decltype(auto) redirect(this auto &&self, Redirect_ &&redirect)
        requires concepts::IsCommandNode<std::decay_t<decltype(self)>>
    {
        return static_cast<std::decay_t<decltype(self)>>(std::move(FWD(self)));
    }
};
} // namespace helium::commands
