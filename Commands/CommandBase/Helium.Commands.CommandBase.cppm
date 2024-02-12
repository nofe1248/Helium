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

export namespace helium::commands {
    struct CommandNodeDescriptor {
        std::shared_ptr<CommandNodeDescriptor> parent_node;
        plf::hive<std::shared_ptr<CommandNodeDescriptor>> child_nodes;
        pro::proxy<poly::CommandNodeFacade> self;
    };

    struct CommandInfo {
        std::string name = "default";
        std::string help_message = "default";
        std::optional<std::string> abbreviated_name = std::nullopt;
    };

    template<typename Derived_>
    class CommandBase : public base::HeliumObject {
    public:
        using Derived = Derived_;

    protected:
        CommandInfo info_;
        std::function<bool()> predicate_;
        std::function<void(CommandContext const &)> callback_;
        std::shared_ptr<CommandNodeDescriptor> node_descriptor_;

        constexpr auto setProxy(this auto &&self) -> void {
            FWD(self).node_descriptor_->self = pro::make_proxy<poly::CommandNodeFacade>(FWD(self));
        }

        template<std::invocable Pred_>
        constexpr auto addPredicate(this auto &&self, Pred_ &&pred) -> void {
            FWD(self).predicate_ = FWD(pred);
        }

        template<std::invocable<CommandContext const &> Callback_>
        constexpr auto addCallback(this auto &&self, Callback_ &&callback) -> void {
            FWD(self).callback_ = FWD(callback);
        }

    public:
        constexpr CommandBase(CommandInfo info) :
            info_(std::move(info)), node_descriptor_(std::make_shared<CommandNodeDescriptor>()) {}
        constexpr CommandBase(std::string command_name, std::string command_help_message = "default",
                    std::optional<std::string> command_abbreviated_name = std::nullopt) :
            info_{.name = std::move(command_name),
                  .help_message = std::move(command_help_message),
                  .abbreviated_name = std::move(command_abbreviated_name)},
            node_descriptor_(std::make_shared<CommandNodeDescriptor>()) {}
        constexpr CommandBase() = delete;
        constexpr CommandBase(CommandBase const &) = default;
        constexpr CommandBase(CommandBase &&) noexcept = default;

        constexpr auto operator=(CommandBase const &) -> CommandBase & = default;
        constexpr auto operator=(CommandBase &&) noexcept -> CommandBase & = default;

        [[nodiscard]] auto getNodeDescriptor(this auto &&self) -> std::weak_ptr<CommandNodeDescriptor> {
            return FWD(self).node_descriptor_;
        }
        auto setParentNode(this auto &&self, std::weak_ptr<CommandNodeDescriptor> const parent) -> void {
            if (auto ptr = parent.lock()) {
                FWD(self).node_descriptor_->parent_node = ptr;
            }
        }
        auto addChildNode(this auto &&self, std::weak_ptr<CommandNodeDescriptor> const child) -> void {
            if (auto ptr = child.lock()) {
                FWD(self).node_descriptor_->child_nodes.insert(ptr);
            }
        }

        template<typename Next_>
        [[nodiscard]] constexpr decltype(auto) then(this auto &&self, Next_ &&next_node) {
            FWD(next_node).setParentNode(FWD(self).getNodeDescriptor());
            FWD(self).addChildNode(FWD(next_node).getNodeDescriptor());
            return next_node;
        }

        template<std::invocable<CommandContext const &> Callback_>
        [[nodiscard]] constexpr decltype(auto) execute(this auto &&self, Callback_ &&callback) {
            FWD(self).addCallback(FWD(callback));
            return Derived(std::move(FWD(self)));
        }

        template<std::invocable Pred_>
        [[nodiscard]] constexpr decltype(auto) require(this auto &&self, Pred_ &&pred) {
            FWD(self).addPredicate(FWD(pred));
            return Derived(std::move(FWD(self)));
        }

        template<concepts::IsCommandNode Redirect_>
        [[nodiscard]] constexpr decltype(auto) redirect(this auto &&self, Redirect_ &&redirect) {
            return Derived(std::move(FWD(self)));
        }
    };
} // namespace helium::commands
