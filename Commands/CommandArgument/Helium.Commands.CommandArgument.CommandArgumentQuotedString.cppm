/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <memory>
#include <string>

#include <proxy/proxy.h>

export module Helium.Commands.CommandArgument.CommandArgumentQuotedString;

import Helium.Base;
import Helium.Commands.CommandBase;
import Helium.Commands.Concepts;

export namespace helium::commands {
	template <concepts::IsString StrType_>
	class CommandArgumentQuotedString
		: public CommandBase<CommandArgumentQuotedString<StrType_>>, public details::TagCommandArgument, public HeliumObject
    {
	public:
		using StringType = StrType_;
		using super = CommandBase<CommandArgumentQuotedString>;
	
	private:
		std::shared_ptr<CommandNodeDescriptor> node_descriptor_;

	public:
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

		CommandArgumentQuotedString()
			:
			node_descriptor_(std::make_shared<CommandNodeDescriptor>())
		{
			this->node_descriptor_->self = pro::make_proxy<poly::CommandNodeFacade>(*this);
		}
	};
}