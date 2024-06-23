/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <memory>
#include <optional>
#include <plf_hive.h>
#include <string>
#include <utility>

#define FWD(x) ::std::forward<decltype(x)>(x)

#include <proxy/proxy.h>

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

    std::optional<std::function<bool()>> node_predicate = std::nullopt;
    std::optional<std::function<void(CommandContext const &)>> node_callback = std::nullopt;

    std::function<bool(Token const &)> try_accept_token;

    CommandNodeDescriptor(std::string command_name, std::string command_description, std::optional<std::string> command_abbreviated_name = std::nullopt)
        : node_name(std::move(command_name)), node_description(std::move(command_description)), node_abbreviated_name(std::move(command_abbreviated_name))
    {
    }

    constexpr CommandNodeDescriptor() = default;
    constexpr CommandNodeDescriptor(CommandNodeDescriptor const &) = default;
    constexpr CommandNodeDescriptor(CommandNodeDescriptor &&) noexcept = default;

    constexpr auto operator=(CommandNodeDescriptor const &) -> CommandNodeDescriptor & = default;
    constexpr auto operator=(CommandNodeDescriptor &&) noexcept -> CommandNodeDescriptor & = default;
};

class CommandNodeBase : public base::HeliumObject
{
protected:
    std::shared_ptr<CommandNodeDescriptor> node_descriptor_;

    template <std::invocable Pred_> constexpr auto addPredicate(this auto &&self, Pred_ &&pred) -> void
    {
        FWD(self).node_descriptor_->node_predicaate.insert(FWD(pred));
    }

    template <std::invocable<CommandContext const &> Callback_> constexpr auto addCallback(this auto &&self, Callback_ &&callback) -> void
    {
        FWD(self).node_descriptor->parent_node = FWD(callback);
    }

private:
    constexpr auto tryInitCommandNode(this auto&& self) -> void
    {
        using SelfType = std::decay_t<decltype(self)>;
        if constexpr(requires(SelfType s) { s.initCommandNode(); })
        {
            FWD(self).initCommandNode();
        }
    }

public:
    constexpr CommandNodeBase(std::string command_name, std::string command_description, std::optional<std::string> command_abbreviated_name = std::nullopt)
    {
        this->node_descriptor_->node_name = command_name;
        this->node_descriptor_->node_description = command_description;
        this->node_descriptor_->node_abbreviated_name = command_abbreviated_name;
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
    {
        return FWD(self).node_descriptor_;
    }
    auto setParentNode(this auto &&self, std::weak_ptr<CommandNodeDescriptor> const parent) -> void
    {
        if (auto ptr = parent.lock())
        {
            FWD(self).node_descriptor_->parent_node = ptr;
        }
    }
    auto addChildNode(this auto &&self, std::weak_ptr<CommandNodeDescriptor> const child) -> void
    {
        if (auto ptr = child.lock())
        {
            FWD(self).node_descriptor_->child_nodes.insert(ptr);
        }
    }

    template <typename Next_> [[nodiscard]] constexpr decltype(auto) then(this auto &&self, Next_ &&next_node)
    {
        FWD(next_node).setParentNode(FWD(self).getNodeDescriptor());
        FWD(self).addChildNode(FWD(next_node).getNodeDescriptor());
        return next_node;
    }

    template <std::invocable<CommandContext const &> Callback_> [[nodiscard]] constexpr decltype(auto) execute(this auto &&self, Callback_ &&callback)
    {
        FWD(self).addCallback(FWD(callback));
        return Derived(std::move(FWD(self)));
    }

    template <std::invocable Pred_> [[nodiscard]] constexpr decltype(auto) require(this auto &&self, Pred_ &&pred)
    {
        FWD(self).addPredicate(FWD(pred));
        return Derived(std::move(FWD(self)));
    }

    template <concepts::IsCommandNode Redirect_> [[nodiscard]] constexpr decltype(auto) redirect(this auto &&self, Redirect_ &&redirect)
    {
        return Derived(std::move(FWD(self)));
    }
};
} // namespace helium::commands
