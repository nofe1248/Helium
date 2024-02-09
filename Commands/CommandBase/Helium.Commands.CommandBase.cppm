/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <memory>
#include <utility>
#include <string>

#include <plf_hive.h>

#define FWD(x) ::std::forward<decltype(x)>(x)

#include <proxy/proxy.h>

export module Helium.Commands.CommandBase;

import Helium.Base;
import Helium.Commands.Concepts;

namespace helium::commands::details {
	template <typename Derived_>
	class CRTPHelper {
	public:
		using Derived = Derived_;
		auto underlyingClass() -> Derived& { return static_cast<Derived&>(*this); }
		auto underlyingClass() const -> Derived const& { return static_cast<Derived const&>(*this); }
	};
}

export namespace helium::commands {
	struct CommandNodeDescriptor {
		std::shared_ptr<CommandNodeDescriptor> parent_node;
		plf::hive<std::shared_ptr<CommandNodeDescriptor>> child_nodes;
		pro::proxy<poly::CommandNodeFacade> self;
	};

	template <typename Derived_>
	class CommandBase : public details::CRTPHelper<Derived_>, public HeliumObject {
	public:
		using Derived = Derived_;

		template <typename Next_>
		[[nodiscard]]
		constexpr auto then(Next_&& next_node) && {
			FWD(next_node).setParentNode(this->underlyingClass().getNodeDescriptor());
			this->underlyingClass().addChildNode(FWD(next_node).getNodeDescriptor());
			return next_node;
		}

		template <std::invocable Callback_>
		[[nodiscard]]
		constexpr auto execute(Callback_&& callback) && {
		    this->underlyingClass().addCallback(FWD(callback));
			return Derived(std::move(this->underlyingClass()));
		}

		template <std::invocable Pred_>
		[[nodiscard]]
		constexpr auto require(Pred_&& pred) && {
		    this->underlyingClass().addPredicate(FWD(pred));
			return Derived(std::move(this->underlyingClass()));
		}
	};
}