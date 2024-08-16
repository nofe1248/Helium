/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <memory>
#include <optional>

export module Helium.Commands.CommandRoot;

import Helium.Commands.CommandBase;
import Helium.Commands.Lexer;

export namespace helium::commands {
    class CommandRoot : public CommandNodeBase
    {
    public:
        using CommandNodeBase::CommandNodeBase;

        constexpr CommandRoot() : CommandNodeBase("root_node")
        {
        }

        static auto tryAcceptToken(std::shared_ptr<CommandNodeDescriptor> node_descriptor, Token const &tok) noexcept -> TryAcceptTokenResult
        {
            return TryAcceptTokenResult{.accepted = false, .argument = std::nullopt};
        }
        static auto tokenSimilarity(std::shared_ptr<CommandNodeDescriptor> node_descriptor, Token const &tok) noexcept -> double
        {
            return 0.0f;
        }
    };
}