module;

#include <memory>

export module Helium.Commands.CommandRoot;

import Helium.Commands.CommandBase;
import Helium.Commands.Lexer;

export namespace helium::commands {
    class CommandRoot : public CommandNodeBase
    {
    public:
        using CommandNodeBase::CommandNodeBase;

        constexpr CommandRoot() : CommandNodeBase("root_node") {}

        static auto tryAcceptToken(std::shared_ptr<CommandNodeDescriptor> node_descriptor, Token const &tok) noexcept -> bool
        {
            return false;
        }
        static auto tokenSimilarity(std::shared_ptr<CommandNodeDescriptor> node_descriptor, Token const &tok) noexcept -> std::size_t
        {
            return 0;
        }
    };
}