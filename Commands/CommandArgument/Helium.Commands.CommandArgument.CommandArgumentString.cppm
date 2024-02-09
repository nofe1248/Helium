/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <concepts>
#include <functional>
#include <memory>
#include <string>

#include <proxy/proxy.h>

#define FWD(x) ::std::forward<decltype(x)>(x)

export module Helium.Commands.CommandArgument.CommandArgumentString;

import Helium.Base;
import Helium.Commands.CommandBase;
import Helium.Commands.CommandContext;
import Helium.Commands.Concepts;

export namespace helium::commands {
	template <concepts::IsString StrType_>
	class CommandArgumentString
		: public CommandBase<CommandArgumentString<StrType_>>, public details::TagCommandArgument
	{
	public:
		using StringType = StrType_;
		using super = CommandBase<CommandArgumentString>;

	private:
		std::shared_ptr<CommandNodeDescriptor> node_descriptor_;
	    std::function<bool()> predicate_;
	    std::function<void(CommandContext const&)> callback_;

	public:
	    template <std::invocable Pred_>
        auto addPredicate(Pred_ && pred) -> void {
	        this->predicate_ = FWD(pred);
	    }

	    template <std::invocable Callback_>
        auto addCallback(Callback_ && callback) -> void {
	        this->callback_ = FWD(callback_);
	    }

		auto getNodeDescriptor() const -> std::weak_ptr<CommandNodeDescriptor> { 
			return this->node_descriptor_; 
		}
		auto setParentNode(std::weak_ptr<CommandNodeDescriptor> parent) -> void { 
			if(auto ptr = parent.lock()) {
				this->node_descriptor_->parent_node = ptr; 
			}
		}
		auto addChildNode(std::weak_ptr<CommandNodeDescriptor> child) -> void {
			if(auto ptr = child.lock()) {
				this->node_descriptor_->child_nodes.insert(ptr);
			}
		}

		auto tryAcceptCommand(std::string_view cmd) -> void {

		}

		CommandArgumentString()
			:
			node_descriptor_(std::make_shared<CommandNodeDescriptor>())
		{
			this->node_descriptor_->self = pro::make_proxy<poly::CommandNodeFacade>(*this);
		}
	};
}