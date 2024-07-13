/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <pybind11/chrono.h>
#include <pybind11/embed.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>

export module Helium.Plugins.Bindings.Python;

import Helium.CLI;
import Helium.Commands;
import Helium.Config;
import Helium.Logger;

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(helium, m)
{
    using namespace helium;

    m.doc() = "Python bindings for Helium plugins";

    auto logger_module = m.def_submodule("logger");
    py::class_<logger::LoggerImpl, std::shared_ptr<logger::LoggerImpl>>(logger_module, "Logger")
        .def(py::init([](std::string const &name, std::string const &thread) { return std::make_shared<logger::LoggerImpl>(name, thread); }))
        .def("trace", &logger::LoggerImpl::trace_string)
        .def("debug", &logger::LoggerImpl::debug_string)
        .def("info", &logger::LoggerImpl::info_string)
        .def("warn", &logger::LoggerImpl::warn_string)
        .def("error", &logger::LoggerImpl::error_string)
        .def("critical", &logger::LoggerImpl::critical_string)
        .def("flush", &logger::LoggerImpl::flush)
        .def("enable_debug_log", &logger::LoggerImpl::enableDebugLog)
        .def("disable_debug_log", &logger::LoggerImpl::disableDebugLog);

    auto config_module = m.def_submodule("config");
    py::enum_<config::ServerType>(config_module, "ServerType")
        .value("Vanilla", config::ServerType::VANILLA)
        .value("Beta18", config::ServerType::BETA18)
        .value("BukkitLegacy", config::ServerType::BUKKIT_LEGACY)
        .value("Bukkit", config::ServerType::BUKKIT)
        .value("Forge", config::ServerType::FORGE)
        .value("CatServer", config::ServerType::CAT_SERVER)
        .value("Arclight", config::ServerType::ARCLIGHT)
        .value("Bungeecord", config::ServerType::BUNGEECORD)
        .value("Waterfall", config::ServerType::WATERFALL)
        .value("Velocity", config::ServerType::VELOCITY)
        .export_values();

    py::class_<config::ServerRCONConfig>(config_module, "ServerRCONConfig")
        .def_readwrite("enable", &config::ServerRCONConfig::enable)
        .def_readwrite("address", &config::ServerRCONConfig::address)
        .def_readwrite("port", &config::ServerRCONConfig::port)
        .def_readwrite("password", &config::ServerRCONConfig::password);

    py::class_<config::DebugConfig>(config_module, "DebugConfig").def_readwrite("debug_mode", &config::DebugConfig::debug_mode);

    py::class_<config::ServerConfig>(config_module, "ServerConfig")
        .def_readwrite("path", &config::ServerConfig::path)
        .def_readwrite("type", &config::ServerConfig::type)
        .def_readwrite("startup_command", &config::ServerConfig::startup_command)
        .def_readwrite("rcon", &config::ServerConfig::rcon);

    py::class_<config::PythonPluginConfig>(config_module, "PythonPluginConfig").def_readwrite("path", &config::PythonPluginConfig::path);

    py::class_<config::HeliumConfig>(config_module, "HeliumConfig")
        .def_readwrite("python_plugin", &config::HeliumConfig::python_plugin)
        .def_readwrite("server", &config::HeliumConfig::server)
        .def_readwrite("debug", &config::HeliumConfig::debug);

    config_module.def("read_config", []() -> config::HeliumConfig & {
        config::readConfig();
        return config::config;
    });
    config_module.def("save_config", []() -> config::HeliumConfig & {
        config::saveConfig();
        return config::config;
    });
    config_module.def("get_config", []() -> config::HeliumConfig & {
        return config::config;
    });

    auto command_module = m.def_submodule("command");
    py::class_<commands::CommandSource>(command_module, "CommandSource")
        .def(py::init<std::string, std::string, std::optional<std::string>>(), py::arg("source_type"), py::arg("major_source"),
             py::arg("minor_source") = py::none{})
        .def(py::init<>());

    py::class_<commands::CommandContext>(command_module, "CommandContext").def(py::init<commands::CommandSource>());

    py::class_<commands::CommandNodeDescriptor, std::shared_ptr<commands::CommandNodeDescriptor>>(command_module, "CommandNodeDescriptor")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{}, py::arg("abbreviated_name") = py::none{});

    py::class_<commands::bindings::AbstractCommandNodeBinding>(command_module, "AbstractCommandNode")
        .def("then", &commands::bindings::AbstractCommandNodeBinding::then)
        .def("execute", &commands::bindings::AbstractCommandNodeBinding::execute)
        .def("require", &commands::bindings::AbstractCommandNodeBinding::require)
        .def("fork", &commands::bindings::AbstractCommandNodeBinding::fork)
        .def("redirect", &commands::bindings::AbstractCommandNodeBinding::redirect);

    py::class_<commands::bindings::CommandLiteralBaseBinding, commands::bindings::AbstractCommandNodeBinding>(command_module, "CommandLiteralBase")
        .def("then", &commands::bindings::AbstractCommandNodeBinding::then)
        .def("execute", &commands::bindings::AbstractCommandNodeBinding::execute)
        .def("require", &commands::bindings::AbstractCommandNodeBinding::require)
        .def("fork", &commands::bindings::AbstractCommandNodeBinding::fork)
        .def("redirect", &commands::bindings::AbstractCommandNodeBinding::redirect);

    py::class_<commands::bindings::CommandLiteralStringBinding, commands::bindings::CommandLiteralBaseBinding>(command_module, "CommandLiteralString")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{})
        .def("then", &commands::bindings::AbstractCommandNodeBinding::then)
        .def("execute", &commands::bindings::AbstractCommandNodeBinding::execute)
        .def("require", &commands::bindings::AbstractCommandNodeBinding::require)
        .def("fork", &commands::bindings::AbstractCommandNodeBinding::fork)
        .def("redirect", &commands::bindings::AbstractCommandNodeBinding::redirect);

    py::class_<commands::bindings::CommandArgumentBaseBinding, commands::bindings::AbstractCommandNodeBinding>(command_module, "CommandArgumentBase")
        .def("then", &commands::bindings::AbstractCommandNodeBinding::then)
        .def("execute", &commands::bindings::AbstractCommandNodeBinding::execute)
        .def("require", &commands::bindings::AbstractCommandNodeBinding::require)
        .def("fork", &commands::bindings::AbstractCommandNodeBinding::fork)
        .def("redirect", &commands::bindings::AbstractCommandNodeBinding::redirect);

    py::class_<commands::bindings::CommandArgumentBooleanBinding, commands::bindings::CommandArgumentBaseBinding>(command_module,
                                                                                                                  "CommandArgumentBoolean")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{})
        .def("then", &commands::bindings::AbstractCommandNodeBinding::then)
        .def("execute", &commands::bindings::AbstractCommandNodeBinding::execute)
        .def("require", &commands::bindings::AbstractCommandNodeBinding::require)
        .def("fork", &commands::bindings::AbstractCommandNodeBinding::fork)
        .def("redirect", &commands::bindings::AbstractCommandNodeBinding::redirect);

    py::class_<commands::bindings::CommandArgumentIntegerBinding, commands::bindings::CommandArgumentBaseBinding>(command_module,
                                                                                                                  "CommandArgumentInteger")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{})
        .def("then", &commands::bindings::AbstractCommandNodeBinding::then)
        .def("execute", &commands::bindings::AbstractCommandNodeBinding::execute)
        .def("require", &commands::bindings::AbstractCommandNodeBinding::require)
        .def("fork", &commands::bindings::AbstractCommandNodeBinding::fork)
        .def("redirect", &commands::bindings::AbstractCommandNodeBinding::redirect);

    py::class_<commands::bindings::CommandArgumentFloatingPointBinding, commands::bindings::CommandArgumentBaseBinding>(
        command_module, "CommandArgumentFloatingPoint")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{})
        .def("then", &commands::bindings::AbstractCommandNodeBinding::then)
        .def("execute", &commands::bindings::AbstractCommandNodeBinding::execute)
        .def("require", &commands::bindings::AbstractCommandNodeBinding::require)
        .def("fork", &commands::bindings::AbstractCommandNodeBinding::fork)
        .def("redirect", &commands::bindings::AbstractCommandNodeBinding::redirect);

    py::class_<commands::bindings::CommandArgumentStringBinding, commands::bindings::CommandArgumentBaseBinding>(command_module,
                                                                                                                 "CommandArgumentString")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{})
        .def("then", &commands::bindings::AbstractCommandNodeBinding::then)
        .def("execute", &commands::bindings::AbstractCommandNodeBinding::execute)
        .def("require", &commands::bindings::AbstractCommandNodeBinding::require)
        .def("fork", &commands::bindings::AbstractCommandNodeBinding::fork)
        .def("redirect", &commands::bindings::AbstractCommandNodeBinding::redirect);

    py::class_<commands::bindings::CommandArgumentQuotedStringBinding, commands::bindings::CommandArgumentBaseBinding>(command_module,
                                                                                                                       "CommandArgumentQuotedString")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{})
        .def("then", &commands::bindings::AbstractCommandNodeBinding::then)
        .def("execute", &commands::bindings::AbstractCommandNodeBinding::execute)
        .def("require", &commands::bindings::AbstractCommandNodeBinding::require)
        .def("fork", &commands::bindings::AbstractCommandNodeBinding::fork)
        .def("redirect", &commands::bindings::AbstractCommandNodeBinding::redirect);

    py::class_<commands::bindings::CommandDispatcherBinding>(command_module, "CommandDispatcher")
        .def(py::init<>())
        .def("register_command", &commands::bindings::CommandDispatcherBinding::registerCommand)
        .def("try_execute_command", &commands::bindings::CommandDispatcherBinding::tryExecuteCommand)
        .def("get_suggestions", &commands::bindings::CommandDispatcherBinding::getSuggestions, py::arg("command"), py::arg("similarity_cutoff"),
             py::arg("show_all_results") = py::bool_{false});

    command_module.def("register_command", [](commands::bindings::AbstractCommandNodeBinding &node) {
        cli::dispatcher.registerRawCommandNodeDescriptor(node.getNodeDescriptor());
    });
    command_module.def("try_execute_command", [](commands::CommandSource const &source, std::string const &command) -> bool {
        return cli::dispatcher.tryExecuteCommand(source, command);
    });
    command_module.def("get_suggestions",
                       [](std::string const &command, double similarity_cutoff, bool show_all_result = false) -> std::vector<std::string> {
                           return cli::dispatcher.getSuggestions(command, similarity_cutoff, show_all_result);
                       });
}