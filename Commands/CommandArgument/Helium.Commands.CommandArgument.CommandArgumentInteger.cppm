/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <cstdint>
#include <concepts>
#include <functional>
#include <memory>
#include <string>
#include <limits>

#include <proxy/proxy.h>

#define FWD(x) ::std::forward<decltype(x)>(x)

export module Helium.Commands.CommandArgument.CommandArgumentInteger;

import Helium.Base;
import Helium.Commands.Concepts;
import Helium.Commands.CommandContext;
import Helium.Commands.CommandBase;

export namespace helium::commands {
	template <concepts::IsInteger IntegerType = std::int64_t>
	struct IntegerBoundMin { IntegerType value = std::numeric_limits<IntegerType>::min(); };

	template <concepts::IsInteger IntegerType = std::int64_t>
	struct IntegerBoundMax { IntegerType value = std::numeric_limits<IntegerType>::max(); };

	template <concepts::IsInteger IntegerType_ = std::int64_t>
	class IntegerBound
    {
	public:
		using IntegerType = IntegerType_;
		using IntegerBoundMaxType = IntegerBoundMax<IntegerType>;
		using IntegerBoundMinType = IntegerBoundMin<IntegerType>;

	private:
		IntegerBoundMaxType max_{};
		IntegerBoundMinType min_{};

	public:
		constexpr explicit IntegerBound(IntegerBoundMaxType max, IntegerBoundMinType min) : max_(max), min_(min)
		{}
		constexpr explicit IntegerBound(IntegerBoundMinType min) : IntegerBound(IntegerBoundMaxType{}, min)
		{}
		constexpr explicit IntegerBound(IntegerBoundMaxType max) : IntegerBound(max, IntegerBoundMinType{})
		{}
		constexpr explicit IntegerBound(IntegerBoundMinType min, IntegerBoundMaxType max) : IntegerBound(max, min) 
		{}
		constexpr explicit IntegerBound() : IntegerBound(IntegerBoundMaxType{}, IntegerBoundMinType{})
		{}
	};

	template <concepts::IsInteger IntegerType_ = std::int64_t>
	class CommandArgumentInteger
		: public CommandBase<CommandArgumentInteger<IntegerType_>>, public details::TagCommandArgument
    {
	public:
		using IntegerType = IntegerType_;
		using IntegerBoundType = IntegerBound<IntegerType>;
		using super = CommandBase<CommandArgumentInteger<IntegerType>>;

	private:
		IntegerBoundType bound_{};
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

		CommandArgumentInteger(IntegerBoundType bound = IntegerBoundType{})
			: 
			bound_(bound),
			node_descriptor_(std::make_shared<CommandNodeDescriptor>())
		{
			this->node_descriptor_->self = pro::make_proxy<poly::CommandNodeFacade>(*this);
		}
	};
}