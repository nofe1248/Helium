/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <any>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <pybind11/chrono.h>
#include <pybind11/embed.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>

export module Helium.Commands.BindingHelper;

import Helium.Base.HeliumObject;
import Helium.Commands.CommandBase;
import Helium.Commands.CommandDispatcher;
import Helium.Commands.CommandLiteral;
import Helium.Commands.CommandArgument;
import Helium.Commands.CommandContext;
import Helium.Commands.CommandSource;
import Helium.Utils;

namespace py = pybind11;

export namespace helium::commands::bindings
{
class AbstractCommandNodeBinding
{
public:
    AbstractCommandNodeBinding(std::string name, std::optional<std::string> description = std::nullopt,
                               std::optional<std::string> abbreviated_name = std::nullopt)
    {
    }
    virtual ~AbstractCommandNodeBinding() = default;

    virtual auto getNodeDescriptor() -> std::shared_ptr<CommandNodeDescriptor>
    {
        return nullptr;
    }
    virtual auto then(AbstractCommandNodeBinding &next_node) -> AbstractCommandNodeBinding &
    {
        return *this;
    }
    virtual auto then(std::vector<AbstractCommandNodeBinding> &next_node) -> AbstractCommandNodeBinding &
    {
        return *this;
    }
    virtual auto execute(py::function callback) -> AbstractCommandNodeBinding &
    {
        return *this;
    }
    virtual auto require(py::function callback) -> AbstractCommandNodeBinding &
    {
        return *this;
    }
    virtual auto fork(AbstractCommandNodeBinding &fork_node) -> AbstractCommandNodeBinding &
    {
        return *this;
    }
    virtual auto fork(std::vector<AbstractCommandNodeBinding> &fork_node) -> AbstractCommandNodeBinding &
    {
        return *this;
    }
    virtual auto redirect(AbstractCommandNodeBinding &redirect_node) -> AbstractCommandNodeBinding &
    {
        return *this;
    }
    virtual auto redirect(std::vector<AbstractCommandNodeBinding> &redirect_node) -> AbstractCommandNodeBinding &
    {
        return *this;
    }
};

class CommandLiteralBaseBinding : public AbstractCommandNodeBinding
{
    using AbstractCommandNodeBinding::AbstractCommandNodeBinding;
};

class CommandLiteralStringBinding : public CommandLiteralBaseBinding
{
private:
    CommandStringLiteral real_node_;

public:
    CommandLiteralStringBinding(std::string name, std::optional<std::string> description = std::nullopt,
                                std::optional<std::string> abbreviated_name = std::nullopt)
        : CommandLiteralBaseBinding(name, description, abbreviated_name),
          real_node_(std::move(name), std::move(description), std::move(abbreviated_name))
    {
    }

    auto getNodeDescriptor() -> std::shared_ptr<CommandNodeDescriptor> override
    {
        return this->real_node_.getNodeDescriptor().lock();
    }
    auto then(AbstractCommandNodeBinding &next_node) -> AbstractCommandNodeBinding & override
    {
        this->real_node_.addChildNode(next_node.getNodeDescriptor());
        return *this;
    }
    auto then(std::vector<AbstractCommandNodeBinding> &next_node) -> AbstractCommandNodeBinding & override
    {
        for (auto &node : next_node)
        {
            this->real_node_.addChildNode(node.getNodeDescriptor());
        }
        return *this;
    }
    auto execute(py::function callback) -> AbstractCommandNodeBinding & override
    {
        (void)this->real_node_.execute(std::move([callback](CommandContext const &context) -> void {
            utils::RunLoopExecutor::getInstance().execute([callback, context] { callback(context, py::none{}); });
        }));
        return *this;
    }
    auto require(py::function callback) -> AbstractCommandNodeBinding & override
    {
        (void)this->real_node_.require(std::move([callback](CommandContext const &context) -> bool {
            return utils::RunLoopExecutor::getInstance().execute<bool>(
                [callback, context]() -> bool { return callback(context, py::none{}).cast<bool>(); });
        }));
        return *this;
    }
    auto fork(AbstractCommandNodeBinding &fork_node) -> AbstractCommandNodeBinding & override
    {
        this->real_node_.addForwardNode(fork_node.getNodeDescriptor());
        return *this;
    }
    auto fork(std::vector<AbstractCommandNodeBinding> &fork_node) -> AbstractCommandNodeBinding & override
    {
        for (auto &node : fork_node)
        {
            this->real_node_.addForwardNode(node.getNodeDescriptor());
        }
        return *this;
    }
    auto redirect(AbstractCommandNodeBinding &redirect_node) -> AbstractCommandNodeBinding & override
    {
        this->real_node_.getNodeDescriptor().lock()->is_redirected = true;
        this->real_node_.addForwardNode(redirect_node.getNodeDescriptor());
        return *this;
    }
    auto redirect(std::vector<AbstractCommandNodeBinding> &redirect_node) -> AbstractCommandNodeBinding & override
    {
        this->real_node_.getNodeDescriptor().lock()->is_redirected = true;
        for (auto &node : redirect_node)
        {
            this->real_node_.addForwardNode(node.getNodeDescriptor());
        }
        return *this;
    }
};

class CommandArgumentBaseBinding : public AbstractCommandNodeBinding
{
    using AbstractCommandNodeBinding::AbstractCommandNodeBinding;
};

class CommandArgumentBooleanBinding : public CommandArgumentBaseBinding
{
private:
    CommandArgumentBoolean real_node_;

public:
    CommandArgumentBooleanBinding(std::string name, std::optional<std::string> description = std::nullopt,
                                  std::optional<std::string> abbreviated_name = std::nullopt)
        : CommandArgumentBaseBinding(name, description, abbreviated_name),
          real_node_(std::move(name), std::move(description), std::move(abbreviated_name))
    {
    }

    auto getNodeDescriptor() -> std::shared_ptr<CommandNodeDescriptor> override
    {
        return this->real_node_.getNodeDescriptor().lock();
    }
    auto then(AbstractCommandNodeBinding &next_node) -> AbstractCommandNodeBinding & override
    {
        this->real_node_.addChildNode(next_node.getNodeDescriptor());
        return *this;
    }
    auto then(std::vector<AbstractCommandNodeBinding> &next_node) -> AbstractCommandNodeBinding & override
    {
        for (auto &node : next_node)
        {
            this->real_node_.addChildNode(node.getNodeDescriptor());
        }
        return *this;
    }
    auto execute(py::function callback) -> AbstractCommandNodeBinding & override
    {
        (void)this->real_node_.execute(std::move([callback](CommandContext const &context, bool param) -> void {
            utils::RunLoopExecutor::getInstance().execute([callback, context, param] { callback(context, py::bool_{param}); });
        }));
        return *this;
    }
    auto require(py::function callback) -> AbstractCommandNodeBinding & override
    {
        (void)this->real_node_.require(std::move([callback](CommandContext const &context, bool param) -> bool {
            return utils::RunLoopExecutor::getInstance().execute<bool>(
                [callback, context, param]() -> bool { return callback(context, py::bool_{param}).cast<bool>(); });
        }));
        return *this;
    }
    auto fork(AbstractCommandNodeBinding &fork_node) -> AbstractCommandNodeBinding & override
    {
        this->real_node_.addForwardNode(fork_node.getNodeDescriptor());
        return *this;
    }
    auto fork(std::vector<AbstractCommandNodeBinding> &fork_node) -> AbstractCommandNodeBinding & override
    {
        for (auto &node : fork_node)
        {
            this->real_node_.addForwardNode(node.getNodeDescriptor());
        }
        return *this;
    }
    auto redirect(AbstractCommandNodeBinding &redirect_node) -> AbstractCommandNodeBinding & override
    {
        this->real_node_.getNodeDescriptor().lock()->is_redirected = true;
        this->real_node_.addForwardNode(redirect_node.getNodeDescriptor());
        return *this;
    }
    auto redirect(std::vector<AbstractCommandNodeBinding> &redirect_node) -> AbstractCommandNodeBinding & override
    {
        this->real_node_.getNodeDescriptor().lock()->is_redirected = true;
        for (auto &node : redirect_node)
        {
            this->real_node_.addForwardNode(node.getNodeDescriptor());
        }
        return *this;
    }
};

class CommandArgumentIntegerBinding : public CommandArgumentBaseBinding
{
private:
    CommandArgumentInteger<int_least64_t> real_node_;

public:
    CommandArgumentIntegerBinding(std::string name, std::optional<std::string> description = std::nullopt,
                                  std::optional<std::string> abbreviated_name = std::nullopt)
        : CommandArgumentBaseBinding(name, description, abbreviated_name),
          real_node_(std::move(name), std::move(description), std::move(abbreviated_name))
    {
    }

    auto getNodeDescriptor() -> std::shared_ptr<CommandNodeDescriptor> override
    {
        return this->real_node_.getNodeDescriptor().lock();
    }
    auto then(AbstractCommandNodeBinding &next_node) -> AbstractCommandNodeBinding & override
    {
        this->real_node_.addChildNode(next_node.getNodeDescriptor());
        return *this;
    }
    auto then(std::vector<AbstractCommandNodeBinding> &next_node) -> AbstractCommandNodeBinding & override
    {
        for (auto &node : next_node)
        {
            this->real_node_.addChildNode(node.getNodeDescriptor());
        }
        return *this;
    }
    auto execute(py::function callback) -> AbstractCommandNodeBinding & override
    {
        (void)this->real_node_.execute(std::move([callback](CommandContext const &context, int_least64_t param) -> void {
            utils::RunLoopExecutor::getInstance().execute([callback, context, param] { callback(context, py::int_{param}); });
        }));
        return *this;
    }
    auto require(py::function callback) -> AbstractCommandNodeBinding & override
    {
        (void)this->real_node_.require(std::move([callback](CommandContext const &context, int_least64_t param) -> bool {
            return utils::RunLoopExecutor::getInstance().execute<bool>(
                [callback, context, param]() -> bool { return callback(context, py::int_{param}).cast<bool>(); });
        }));
        return *this;
    }
    auto fork(AbstractCommandNodeBinding &fork_node) -> AbstractCommandNodeBinding & override
    {
        this->real_node_.addForwardNode(fork_node.getNodeDescriptor());
        return *this;
    }
    auto fork(std::vector<AbstractCommandNodeBinding> &fork_node) -> AbstractCommandNodeBinding & override
    {
        for (auto &node : fork_node)
        {
            this->real_node_.addForwardNode(node.getNodeDescriptor());
        }
        return *this;
    }
    auto redirect(AbstractCommandNodeBinding &redirect_node) -> AbstractCommandNodeBinding & override
    {
        this->real_node_.getNodeDescriptor().lock()->is_redirected = true;
        this->real_node_.addForwardNode(redirect_node.getNodeDescriptor());
        return *this;
    }
    auto redirect(std::vector<AbstractCommandNodeBinding> &redirect_node) -> AbstractCommandNodeBinding & override
    {
        this->real_node_.getNodeDescriptor().lock()->is_redirected = true;
        for (auto &node : redirect_node)
        {
            this->real_node_.addForwardNode(node.getNodeDescriptor());
        }
        return *this;
    }
};

class CommandArgumentFloatingPointBinding : public CommandArgumentBaseBinding
{
private:
    CommandArgumentFloatingPoint<double> real_node_;

public:
    CommandArgumentFloatingPointBinding(std::string name, std::optional<std::string> description = std::nullopt,
                                        std::optional<std::string> abbreviated_name = std::nullopt)
        : CommandArgumentBaseBinding(name, description, abbreviated_name),
          real_node_(std::move(name), std::move(description), std::move(abbreviated_name))
    {
    }

    auto getNodeDescriptor() -> std::shared_ptr<CommandNodeDescriptor> override
    {
        return this->real_node_.getNodeDescriptor().lock();
    }
    auto then(AbstractCommandNodeBinding &next_node) -> AbstractCommandNodeBinding & override
    {
        this->real_node_.addChildNode(next_node.getNodeDescriptor());
        return *this;
    }
    auto then(std::vector<AbstractCommandNodeBinding> &next_node) -> AbstractCommandNodeBinding & override
    {
        for (auto &node : next_node)
        {
            this->real_node_.addChildNode(node.getNodeDescriptor());
        }
        return *this;
    }
    auto execute(py::function callback) -> AbstractCommandNodeBinding & override
    {
        (void)this->real_node_.execute(std::move([callback](CommandContext const &context, double param) -> void {
            utils::RunLoopExecutor::getInstance().execute([callback, context, param] { callback(context, py::float_{param}); });
        }));
        return *this;
    }
    auto require(py::function callback) -> AbstractCommandNodeBinding & override
    {
        (void)this->real_node_.require(std::move([callback](CommandContext const &context, double param) -> bool {
            return utils::RunLoopExecutor::getInstance().execute<bool>(
                [callback, context, param]() -> bool { return callback(context, py::float_{param}).cast<bool>(); });
        }));
        return *this;
    }
    auto fork(AbstractCommandNodeBinding &fork_node) -> AbstractCommandNodeBinding & override
    {
        this->real_node_.addForwardNode(fork_node.getNodeDescriptor());
        return *this;
    }
    auto fork(std::vector<AbstractCommandNodeBinding> &fork_node) -> AbstractCommandNodeBinding & override
    {
        for (auto &node : fork_node)
        {
            this->real_node_.addForwardNode(node.getNodeDescriptor());
        }
        return *this;
    }
    auto redirect(AbstractCommandNodeBinding &redirect_node) -> AbstractCommandNodeBinding & override
    {
        this->real_node_.getNodeDescriptor().lock()->is_redirected = true;
        this->real_node_.addForwardNode(redirect_node.getNodeDescriptor());
        return *this;
    }
    auto redirect(std::vector<AbstractCommandNodeBinding> &redirect_node) -> AbstractCommandNodeBinding & override
    {
        this->real_node_.getNodeDescriptor().lock()->is_redirected = true;
        for (auto &node : redirect_node)
        {
            this->real_node_.addForwardNode(node.getNodeDescriptor());
        }
        return *this;
    }
};

class CommandArgumentStringBinding : public CommandArgumentBaseBinding
{
private:
    CommandArgumentString real_node_;

public:
    CommandArgumentStringBinding(std::string name, std::optional<std::string> description = std::nullopt,
                                 std::optional<std::string> abbreviated_name = std::nullopt)
        : CommandArgumentBaseBinding(name, description, abbreviated_name),
          real_node_(std::move(name), std::move(description), std::move(abbreviated_name))
    {
    }

    auto getNodeDescriptor() -> std::shared_ptr<CommandNodeDescriptor> override
    {
        return this->real_node_.getNodeDescriptor().lock();
    }
    auto then(AbstractCommandNodeBinding &next_node) -> AbstractCommandNodeBinding & override
    {
        this->real_node_.addChildNode(next_node.getNodeDescriptor());
        return *this;
    }
    auto then(std::vector<AbstractCommandNodeBinding> &next_node) -> AbstractCommandNodeBinding & override
    {
        for (auto &node : next_node)
        {
            this->real_node_.addChildNode(node.getNodeDescriptor());
        }
        return *this;
    }
    auto execute(py::function callback) -> AbstractCommandNodeBinding & override
    {
        (void)this->real_node_.execute(std::move([callback](CommandContext const &context, std::string param) -> void {
            utils::RunLoopExecutor::getInstance().execute([callback, context, param] { callback(context, py::str{param}); });
        }));
        return *this;
    }
    auto require(py::function callback) -> AbstractCommandNodeBinding & override
    {
        (void)this->real_node_.require(std::move([callback](CommandContext const &context, std::string param) -> bool {
            return utils::RunLoopExecutor::getInstance().execute<bool>(
                [callback, context, param]() -> bool { return callback(context, py::str{param}).cast<bool>(); });
        }));
        return *this;
    }
    auto fork(AbstractCommandNodeBinding &fork_node) -> AbstractCommandNodeBinding & override
    {
        this->real_node_.addForwardNode(fork_node.getNodeDescriptor());
        return *this;
    }
    auto fork(std::vector<AbstractCommandNodeBinding> &fork_node) -> AbstractCommandNodeBinding & override
    {
        for (auto &node : fork_node)
        {
            this->real_node_.addForwardNode(node.getNodeDescriptor());
        }
        return *this;
    }
    auto redirect(AbstractCommandNodeBinding &redirect_node) -> AbstractCommandNodeBinding & override
    {
        this->real_node_.getNodeDescriptor().lock()->is_redirected = true;
        this->real_node_.addForwardNode(redirect_node.getNodeDescriptor());
        return *this;
    }
    auto redirect(std::vector<AbstractCommandNodeBinding> &redirect_node) -> AbstractCommandNodeBinding & override
    {
        this->real_node_.getNodeDescriptor().lock()->is_redirected = true;
        for (auto &node : redirect_node)
        {
            this->real_node_.addForwardNode(node.getNodeDescriptor());
        }
        return *this;
    }
};

class CommandArgumentQuotedStringBinding : public CommandArgumentBaseBinding
{
private:
    CommandArgumentQuotedString real_node_;

public:
    CommandArgumentQuotedStringBinding(std::string name, std::optional<std::string> description = std::nullopt,
                                       std::optional<std::string> abbreviated_name = std::nullopt)
        : CommandArgumentBaseBinding(name, description, abbreviated_name),
          real_node_(std::move(name), std::move(description), std::move(abbreviated_name))
    {
    }

    auto getNodeDescriptor() -> std::shared_ptr<CommandNodeDescriptor> override
    {
        return this->real_node_.getNodeDescriptor().lock();
    }
    auto then(AbstractCommandNodeBinding &next_node) -> AbstractCommandNodeBinding & override
    {
        this->real_node_.addChildNode(next_node.getNodeDescriptor());
        return *this;
    }
    auto then(std::vector<AbstractCommandNodeBinding> &next_node) -> AbstractCommandNodeBinding & override
    {
        for (auto &node : next_node)
        {
            this->real_node_.addChildNode(node.getNodeDescriptor());
        }
        return *this;
    }
    auto execute(py::function callback) -> AbstractCommandNodeBinding & override
    {
        (void)this->real_node_.execute(std::move([callback](CommandContext const &context, std::string param) -> void {
            utils::RunLoopExecutor::getInstance().execute([callback, context, param] { callback(context, py::str{param}); });
        }));
        return *this;
    }
    auto require(py::function callback) -> AbstractCommandNodeBinding & override
    {
        (void)this->real_node_.require(std::move([callback](CommandContext const &context, std::string const &param) -> bool {
            return utils::RunLoopExecutor::getInstance().execute<bool>(
                [callback, context, param]() -> bool { return callback(context, py::str{param}).cast<bool>(); });
        }));
        return *this;
    }
    auto fork(AbstractCommandNodeBinding &fork_node) -> AbstractCommandNodeBinding & override
    {
        this->real_node_.addForwardNode(fork_node.getNodeDescriptor());
        return *this;
    }
    auto fork(std::vector<AbstractCommandNodeBinding> &fork_node) -> AbstractCommandNodeBinding & override
    {
        for (auto &node : fork_node)
        {
            this->real_node_.addForwardNode(node.getNodeDescriptor());
        }
        return *this;
    }
    auto redirect(AbstractCommandNodeBinding &redirect_node) -> AbstractCommandNodeBinding & override
    {
        this->real_node_.getNodeDescriptor().lock()->is_redirected = true;
        this->real_node_.addForwardNode(redirect_node.getNodeDescriptor());
        return *this;
    }
    auto redirect(std::vector<AbstractCommandNodeBinding> &redirect_node) -> AbstractCommandNodeBinding & override
    {
        this->real_node_.getNodeDescriptor().lock()->is_redirected = true;
        for (auto &node : redirect_node)
        {
            this->real_node_.addForwardNode(node.getNodeDescriptor());
        }
        return *this;
    }
};

class CommandDispatcherBinding final
{
private:
    CommandDispatcher dispatcher_;

public:
    CommandDispatcherBinding() = default;
    CommandDispatcherBinding(CommandDispatcherBinding const &) = default;
    CommandDispatcherBinding(CommandDispatcherBinding &&) noexcept = default;

    CommandDispatcherBinding &operator=(CommandDispatcherBinding const &) = default;
    CommandDispatcherBinding &operator=(CommandDispatcherBinding &&) noexcept = default;

    auto registerCommand(AbstractCommandNodeBinding &node) -> void
    {
        this->dispatcher_.registerRawCommandNodeDescriptor(node.getNodeDescriptor());
    }
    auto tryExecuteCommand(CommandSource const &source, std::string const &command) -> bool
    {
        return this->dispatcher_.tryExecuteCommand(source, command);
    }
    auto getSuggestions(std::string const &command, double similarity_cutoff, bool show_all_result = false) -> std::vector<std::string>
    {
        return this->dispatcher_.getSuggestions(command, similarity_cutoff, show_all_result);
    }
};
} // namespace helium::commands::bindings