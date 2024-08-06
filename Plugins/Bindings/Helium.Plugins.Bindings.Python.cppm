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

import Helium.Base;
import Helium.CLI;
import Helium.Commands;
import Helium.Config;
import Helium.Events;
import Helium.Logger;
import Helium.Server;
import Helium.Utils;

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(helium, m)
{
    using namespace helium;

    m.doc() = "Python bindings for Helium plugins";

    auto base_module = m.def_submodule("base");
    base_module.def("get_helium_version_string", &base::getHeliumVersionString);

    auto logger_module = m.def_submodule("logger");
    py::class_<logger::LoggerImpl, std::shared_ptr<logger::LoggerImpl>>(logger_module, "Logger")
        .def(py::init([](std::string const &name, std::string const &thread) { return std::make_shared<logger::LoggerImpl>(name, thread); }),
             py::call_guard<py::gil_scoped_release>())
        .def("trace", &logger::LoggerImpl::trace_string, py::call_guard<py::gil_scoped_release>())
        .def("debug", &logger::LoggerImpl::debug_string, py::call_guard<py::gil_scoped_release>())
        .def("info", &logger::LoggerImpl::info_string, py::call_guard<py::gil_scoped_release>())
        .def("warn", &logger::LoggerImpl::warn_string, py::call_guard<py::gil_scoped_release>())
        .def("error", &logger::LoggerImpl::error_string, py::call_guard<py::gil_scoped_release>())
        .def("critical", &logger::LoggerImpl::critical_string, py::call_guard<py::gil_scoped_release>())
        .def("flush", &logger::LoggerImpl::flush, py::call_guard<py::gil_scoped_release>())
        .def("enable_debug_log", &logger::LoggerImpl::enableDebugLog, py::call_guard<py::gil_scoped_release>())
        .def("disable_debug_log", &logger::LoggerImpl::disableDebugLog, py::call_guard<py::gil_scoped_release>());

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
        .value("Custom", config::ServerType::CUSTOM)
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
        .def_readwrite("startup_command_executable", &config::ServerConfig::startup_command_executable)
        .def_readwrite("startup_command_parameters", &config::ServerConfig::startup_command_parameters)
        .def_readwrite("rcon", &config::ServerConfig::rcon);

    py::class_<config::PythonPluginConfig>(config_module, "PythonPluginConfig").def_readwrite("path", &config::PythonPluginConfig::path);

    py::class_<config::HeliumConfig>(config_module, "HeliumConfig")
        .def_readwrite("python_plugin", &config::HeliumConfig::python_plugin)
        .def_readwrite("server", &config::HeliumConfig::server)
        .def_readwrite("debug", &config::HeliumConfig::debug);

    config_module.def(
        "read_config",
        []() -> config::HeliumConfig & {
            config::readConfig();
            return config::config;
        },
        py::call_guard<py::gil_scoped_release>());
    config_module.def(
        "save_config",
        []() -> config::HeliumConfig & {
            config::saveConfig();
            return config::config;
        },
        py::call_guard<py::gil_scoped_release>());
    config_module.def("get_config", []() -> config::HeliumConfig & { return config::config; }, py::call_guard<py::gil_scoped_release>());

    auto command_module = m.def_submodule("commands");
    py::class_<commands::CommandSource>(command_module, "CommandSource")
        .def(py::init<std::string, std::string, std::optional<std::string>>(), py::arg("source_type"), py::arg("major_source"),
             py::arg("minor_source") = py::none{}, py::call_guard<py::gil_scoped_release>())
        .def(py::init<>(), py::call_guard<py::gil_scoped_release>());

    py::class_<commands::CommandContext>(command_module, "CommandContext")
        .def(py::init<commands::CommandSource>(), py::call_guard<py::gil_scoped_release>());

    py::class_<commands::CommandNodeDescriptor, std::shared_ptr<commands::CommandNodeDescriptor>>(command_module, "CommandNodeDescriptor")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{}, py::call_guard<py::gil_scoped_release>());

    py::class_<commands::bindings::AbstractCommandNodeBinding>(command_module, "AbstractCommandNode")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{}, py::call_guard<py::gil_scoped_release>())
        .def("then", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::AbstractCommandNodeBinding::then),
             py::call_guard<py::gil_scoped_release>())
        .def("then",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(&commands::bindings::AbstractCommandNodeBinding::then),
             py::call_guard<py::gil_scoped_release>())
        .def("execute", &commands::bindings::AbstractCommandNodeBinding::execute, py::call_guard<py::gil_scoped_release>())
        .def("require", &commands::bindings::AbstractCommandNodeBinding::require, py::call_guard<py::gil_scoped_release>())
        .def("fork", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::AbstractCommandNodeBinding::fork),
             py::call_guard<py::gil_scoped_release>())
        .def("fork",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(&commands::bindings::AbstractCommandNodeBinding::fork),
             py::call_guard<py::gil_scoped_release>())
        .def("redirect",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::AbstractCommandNodeBinding::redirect),
             py::call_guard<py::gil_scoped_release>())
        .def("redirect",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                 &commands::bindings::AbstractCommandNodeBinding::redirect),
             py::call_guard<py::gil_scoped_release>());

    py::class_<commands::bindings::CommandLiteralBaseBinding, commands::bindings::AbstractCommandNodeBinding>(command_module, "CommandLiteralBase")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{}, py::call_guard<py::gil_scoped_release>())
        .def("then", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandLiteralBaseBinding::then),
             py::call_guard<py::gil_scoped_release>())
        .def("then",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(&commands::bindings::CommandLiteralBaseBinding::then),
             py::call_guard<py::gil_scoped_release>())
        .def("execute", &commands::bindings::CommandLiteralBaseBinding::execute, py::call_guard<py::gil_scoped_release>())
        .def("require", &commands::bindings::CommandLiteralBaseBinding::require, py::call_guard<py::gil_scoped_release>())
        .def("fork", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandLiteralBaseBinding::fork),
             py::call_guard<py::gil_scoped_release>())
        .def("fork",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(&commands::bindings::CommandLiteralBaseBinding::fork),
             py::call_guard<py::gil_scoped_release>())
        .def("redirect",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandLiteralBaseBinding::redirect),
             py::call_guard<py::gil_scoped_release>())
        .def("redirect",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                 &commands::bindings::CommandLiteralBaseBinding::redirect),
             py::call_guard<py::gil_scoped_release>());

    py::class_<commands::bindings::CommandLiteralStringBinding, commands::bindings::CommandLiteralBaseBinding>(command_module, "CommandLiteralString")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{}, py::call_guard<py::gil_scoped_release>())
        .def("then", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandLiteralStringBinding::then),
             py::call_guard<py::gil_scoped_release>())
        .def("then",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(&commands::bindings::CommandLiteralStringBinding::then),
             py::call_guard<py::gil_scoped_release>())
        .def("execute", &commands::bindings::CommandLiteralStringBinding::execute, py::call_guard<py::gil_scoped_release>())
        .def("require", &commands::bindings::CommandLiteralStringBinding::require, py::call_guard<py::gil_scoped_release>())
        .def("fork", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandLiteralStringBinding::fork),
             py::call_guard<py::gil_scoped_release>())
        .def("fork",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(&commands::bindings::CommandLiteralStringBinding::fork),
             py::call_guard<py::gil_scoped_release>())
        .def("redirect",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandLiteralStringBinding::redirect),
             py::call_guard<py::gil_scoped_release>())
        .def("redirect",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                 &commands::bindings::CommandLiteralStringBinding::redirect),
             py::call_guard<py::gil_scoped_release>());

    py::class_<commands::bindings::CommandArgumentBaseBinding, commands::bindings::AbstractCommandNodeBinding>(command_module, "CommandArgumentBase")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{}, py::call_guard<py::gil_scoped_release>())
        .def("then", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentBaseBinding::then),
             py::call_guard<py::gil_scoped_release>())
        .def("then",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(&commands::bindings::CommandArgumentBaseBinding::then),
             py::call_guard<py::gil_scoped_release>())
        .def("execute", &commands::bindings::CommandArgumentBaseBinding::execute, py::call_guard<py::gil_scoped_release>())
        .def("require", &commands::bindings::CommandArgumentBaseBinding::require, py::call_guard<py::gil_scoped_release>())
        .def("fork", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentBaseBinding::fork),
             py::call_guard<py::gil_scoped_release>())
        .def("fork",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(&commands::bindings::CommandArgumentBaseBinding::fork),
             py::call_guard<py::gil_scoped_release>())
        .def("redirect",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentBaseBinding::redirect),
             py::call_guard<py::gil_scoped_release>())
        .def("redirect",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                 &commands::bindings::CommandArgumentBaseBinding::redirect),
             py::call_guard<py::gil_scoped_release>());

    py::class_<commands::bindings::CommandArgumentBooleanBinding, commands::bindings::CommandArgumentBaseBinding>(command_module,
                                                                                                                  "CommandArgumentBoolean")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{}, py::call_guard<py::gil_scoped_release>())
        .def("then", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentBooleanBinding::then),
             py::call_guard<py::gil_scoped_release>())
        .def("then",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                 &commands::bindings::CommandArgumentBooleanBinding::then),
             py::call_guard<py::gil_scoped_release>())
        .def("execute", &commands::bindings::CommandArgumentBooleanBinding::execute, py::call_guard<py::gil_scoped_release>())
        .def("require", &commands::bindings::CommandArgumentBooleanBinding::require, py::call_guard<py::gil_scoped_release>())
        .def("fork", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentBooleanBinding::fork),
             py::call_guard<py::gil_scoped_release>())
        .def("fork",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                 &commands::bindings::CommandArgumentBooleanBinding::fork),
             py::call_guard<py::gil_scoped_release>())
        .def("redirect",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentBooleanBinding::redirect),
             py::call_guard<py::gil_scoped_release>())
        .def("redirect",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                 &commands::bindings::CommandArgumentBooleanBinding::redirect),
             py::call_guard<py::gil_scoped_release>());

    py::class_<commands::bindings::CommandArgumentIntegerBinding, commands::bindings::CommandArgumentBaseBinding>(command_module,
                                                                                                                  "CommandArgumentInteger")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{}, py::call_guard<py::gil_scoped_release>())
        .def("then", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentIntegerBinding::then),
             py::call_guard<py::gil_scoped_release>())
        .def("then",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                 &commands::bindings::CommandArgumentIntegerBinding::then),
             py::call_guard<py::gil_scoped_release>())
        .def("execute", &commands::bindings::CommandArgumentIntegerBinding::execute, py::call_guard<py::gil_scoped_release>())
        .def("require", &commands::bindings::CommandArgumentIntegerBinding::require, py::call_guard<py::gil_scoped_release>())
        .def("fork", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentIntegerBinding::fork),
             py::call_guard<py::gil_scoped_release>())
        .def("fork",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                 &commands::bindings::CommandArgumentIntegerBinding::fork),
             py::call_guard<py::gil_scoped_release>())
        .def("redirect",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentIntegerBinding::redirect),
             py::call_guard<py::gil_scoped_release>())
        .def("redirect",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                 &commands::bindings::CommandArgumentIntegerBinding::redirect),
             py::call_guard<py::gil_scoped_release>());

    py::class_<commands::bindings::CommandArgumentFloatingPointBinding, commands::bindings::CommandArgumentBaseBinding>(
        command_module, "CommandArgumentFloatingPoint")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{}, py::call_guard<py::gil_scoped_release>())
        .def("then",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentFloatingPointBinding::then),
             py::call_guard<py::gil_scoped_release>())
        .def("then",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                 &commands::bindings::CommandArgumentFloatingPointBinding::then),
             py::call_guard<py::gil_scoped_release>())
        .def("execute", &commands::bindings::CommandArgumentFloatingPointBinding::execute, py::call_guard<py::gil_scoped_release>())
        .def("require", &commands::bindings::CommandArgumentFloatingPointBinding::require, py::call_guard<py::gil_scoped_release>())
        .def("fork",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentFloatingPointBinding::fork),
             py::call_guard<py::gil_scoped_release>())
        .def("fork",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                 &commands::bindings::CommandArgumentFloatingPointBinding::fork),
             py::call_guard<py::gil_scoped_release>())
        .def("redirect",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentFloatingPointBinding::redirect),
             py::call_guard<py::gil_scoped_release>())
        .def("redirect",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                 &commands::bindings::CommandArgumentFloatingPointBinding::redirect),
             py::call_guard<py::gil_scoped_release>());

    py::class_<commands::bindings::CommandArgumentStringBinding, commands::bindings::CommandArgumentBaseBinding>(command_module,
                                                                                                                 "CommandArgumentString")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{}, py::call_guard<py::gil_scoped_release>())
        .def("then", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentStringBinding::then),
             py::call_guard<py::gil_scoped_release>())
        .def(
            "then",
            py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(&commands::bindings::CommandArgumentStringBinding::then),
            py::call_guard<py::gil_scoped_release>())
        .def("execute", &commands::bindings::CommandArgumentStringBinding::execute, py::call_guard<py::gil_scoped_release>())
        .def("require", &commands::bindings::CommandArgumentStringBinding::require, py::call_guard<py::gil_scoped_release>())
        .def("fork", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentStringBinding::fork),
             py::call_guard<py::gil_scoped_release>())
        .def(
            "fork",
            py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(&commands::bindings::CommandArgumentStringBinding::fork),
            py::call_guard<py::gil_scoped_release>())
        .def("redirect",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentStringBinding::redirect),
             py::call_guard<py::gil_scoped_release>())
        .def("redirect",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                 &commands::bindings::CommandArgumentStringBinding::redirect),
             py::call_guard<py::gil_scoped_release>());

    py::class_<commands::bindings::CommandArgumentQuotedStringBinding, commands::bindings::CommandArgumentBaseBinding>(command_module,
                                                                                                                       "CommandArgumentQuotedString")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{}, py::call_guard<py::gil_scoped_release>())
        .def("then",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentQuotedStringBinding::then),
             py::call_guard<py::gil_scoped_release>())
        .def("then",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                 &commands::bindings::CommandArgumentQuotedStringBinding::then),
             py::call_guard<py::gil_scoped_release>())
        .def("execute", &commands::bindings::CommandArgumentQuotedStringBinding::execute, py::call_guard<py::gil_scoped_release>())
        .def("require", &commands::bindings::CommandArgumentQuotedStringBinding::require, py::call_guard<py::gil_scoped_release>())
        .def("fork",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentQuotedStringBinding::fork),
             py::call_guard<py::gil_scoped_release>())
        .def("fork",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                 &commands::bindings::CommandArgumentQuotedStringBinding::fork),
             py::call_guard<py::gil_scoped_release>())
        .def("redirect",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentQuotedStringBinding::redirect),
             py::call_guard<py::gil_scoped_release>())
        .def("redirect",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                 &commands::bindings::CommandArgumentQuotedStringBinding::redirect),
             py::call_guard<py::gil_scoped_release>());

    py::class_<commands::bindings::CommandDispatcherBinding>(command_module, "CommandDispatcher")
        .def(py::init<>(), py::call_guard<py::gil_scoped_release>())
        .def("register_command", &commands::bindings::CommandDispatcherBinding::registerCommand, py::call_guard<py::gil_scoped_release>())
        .def("try_execute_command", &commands::bindings::CommandDispatcherBinding::tryExecuteCommand, py::call_guard<py::gil_scoped_release>())
        .def("get_suggestions", &commands::bindings::CommandDispatcherBinding::getSuggestions, py::arg("command"), py::arg("similarity_cutoff"),
             py::arg("show_all_results") = py::bool_{false}, py::call_guard<py::gil_scoped_release>());

    command_module.def(
        "register_command",
        [](commands::bindings::AbstractCommandNodeBinding &node) { cli::dispatcher.registerRawCommandNodeDescriptor(node.getNodeDescriptor()); },
        py::call_guard<py::gil_scoped_release>());
    command_module.def(
        "try_execute_command",
        [](commands::CommandSource const &source, std::string const &command) -> bool { return cli::dispatcher.tryExecuteCommand(source, command); },
        py::call_guard<py::gil_scoped_release>());
    command_module.def(
        "get_suggestions",
        [](std::string const &command, double similarity_cutoff, bool show_all_result = false) -> std::vector<std::string> {
            return cli::dispatcher.getSuggestions(command, similarity_cutoff, show_all_result);
        },
        py::call_guard<py::gil_scoped_release>());

    auto events_module = m.def_submodule("events");

    py::class_<events::HeliumStarting>(events_module, "HeliumStarting").def(py::init<>(), py::call_guard<py::gil_scoped_release>());
    py::class_<events::HeliumStarted>(events_module, "HeliumStarted").def(py::init<>(), py::call_guard<py::gil_scoped_release>());
    py::class_<events::HeliumStopping>(events_module, "HeliumStopping").def(py::init<>(), py::call_guard<py::gil_scoped_release>());
    py::class_<events::PluginLoaded>(events_module, "PluginLoaded").def(py::init<>(), py::call_guard<py::gil_scoped_release>());
    py::class_<events::PluginUnloaded>(events_module, "PluginUnloaded").def(py::init<>(), py::call_guard<py::gil_scoped_release>());
    py::class_<events::PluginReloaded>(events_module, "PluginReloaded").def(py::init<>(), py::call_guard<py::gil_scoped_release>());
    py::class_<events::ServerStarting>(events_module, "ServerStarting").def(py::init<>(), py::call_guard<py::gil_scoped_release>());
    py::class_<events::ServerStarted>(events_module, "ServerStarted").def(py::init<>(), py::call_guard<py::gil_scoped_release>());
    py::class_<events::ServerStopping>(events_module, "ServerStopping").def(py::init<>(), py::call_guard<py::gil_scoped_release>());
    py::class_<events::ServerStopped>(events_module, "ServerStopped").def(py::init<>(), py::call_guard<py::gil_scoped_release>());
    py::class_<events::ServerPaused>(events_module, "ServerPaused").def(py::init<>(), py::call_guard<py::gil_scoped_release>());
    py::class_<events::ServerResumed>(events_module, "ServerResumed").def(py::init<>(), py::call_guard<py::gil_scoped_release>());
    py::class_<events::ConsoleInput>(events_module, "ConsoleInput")
        .def(py::init<std::string>(), py::call_guard<py::gil_scoped_release>())
        .def_readwrite("input", &events::ConsoleInput::input);
    py::class_<events::ServerOutputRaw>(events_module, "ServerOutputRaw")
        .def(py::init<std::string>(), py::call_guard<py::gil_scoped_release>())
        .def_readwrite("output", &events::ServerOutputRaw::output);
    py::class_<events::PlayerInputRaw>(events_module, "PlayerInputRaw")
        .def(py::init<std::string>(), py::call_guard<py::gil_scoped_release>())
        .def_readwrite("input", &events::PlayerInputRaw::input);
    py::class_<events::ServerOutput>(events_module, "ServerOutput").def(py::init<>(), py::call_guard<py::gil_scoped_release>());
    py::class_<events::PlayerInput>(events_module, "PlayerInput").def(py::init<>(), py::call_guard<py::gil_scoped_release>());
    py::class_<events::PythonEvent>(events_module, "PythonEvent")
        .def(py::init<std::string, py::object>(), py::call_guard<py::gil_scoped_release>())
        .def_readwrite("event_id", &events::PythonEvent::event_id)
        .def_readwrite("event_arg", &events::PythonEvent::event_arg);

    py::enum_<events::binding::HeliumDefaultEventsBindingEnum>(events_module, "DefaultEvents")
        .value("HeliumStarting", events::binding::HeliumDefaultEventsBindingEnum::HELIUM_STARTING)
        .value("HeliumStarted", events::binding::HeliumDefaultEventsBindingEnum::HELIUM_STARTED)
        .value("HeliumStopping", events::binding::HeliumDefaultEventsBindingEnum::HELIUM_STOPPING)
        .value("PluginLoaded", events::binding::HeliumDefaultEventsBindingEnum::PLUGIN_LOADED)
        .value("PluginUnloaded", events::binding::HeliumDefaultEventsBindingEnum::PLUGIN_UNLOADED)
        .value("PluginReloaded", events::binding::HeliumDefaultEventsBindingEnum::PLUGIN_RELOADED)
        .value("ServerStarting", events::binding::HeliumDefaultEventsBindingEnum::SERVER_STARTING)
        .value("ServerStarted", events::binding::HeliumDefaultEventsBindingEnum::SERVER_STARTED)
        .value("ServerStopping", events::binding::HeliumDefaultEventsBindingEnum::SERVER_STOPPING)
        .value("ServerStopped", events::binding::HeliumDefaultEventsBindingEnum::SERVER_STOPPED)
        .value("ServerPaused", events::binding::HeliumDefaultEventsBindingEnum::SERVER_PAUSED)
        .value("ServerResumed", events::binding::HeliumDefaultEventsBindingEnum::SERVER_RESUMED)
        .value("ConsoleInput", events::binding::HeliumDefaultEventsBindingEnum::CONSOLE_INPUT)
        .value("ServerOutputRaw", events::binding::HeliumDefaultEventsBindingEnum::SERVER_OUTPUT_RAW)
        .value("PlayerInputRaw", events::binding::HeliumDefaultEventsBindingEnum::PLAYER_INPUT_RAW)
        .value("ServerOutput", events::binding::HeliumDefaultEventsBindingEnum::SERVER_OUTPUT)
        .value("PlayerInput", events::binding::HeliumDefaultEventsBindingEnum::PLAYER_INPUT)
        .export_values();

    py::class_<events::binding::EventBusBinding>(events_module, "EventBus")
        .def(py::init<>())
        .def("unlisten_all", &events::binding::EventBusBinding::unlistenAll, py::call_guard<py::gil_scoped_release>())
        .def("process_events", &events::binding::EventBusBinding::processEvents, py::call_guard<py::gil_scoped_release>());
    py::class_<events::binding::EventEmitterBinding>(events_module, "EventEmitter")
        .def(py::init<events::binding::EventBusBinding const &>(), py::call_guard<py::gil_scoped_release>())
        .def(py::init<>(), py::call_guard<py::gil_scoped_release>())
        .def("postpone_default_event", &events::binding::EventEmitterBinding::postponeDefaultEvent, py::arg("event_type"),
             py::arg("event_arg") = py::none{}, py::call_guard<py::gil_scoped_release>())
        .def("postpone_custom_event", &events::binding::EventEmitterBinding::postponeCustomEvent, py::call_guard<py::gil_scoped_release>());
    py::class_<events::binding::EventListenerBinding>(events_module, "EventListener")
        .def(py::init<events::binding::EventBusBinding const &>(), py::call_guard<py::gil_scoped_release>())
        .def(py::init<>(), py::call_guard<py::gil_scoped_release>())
        .def("listen_to_default_event", &events::binding::EventListenerBinding::listenToDefaultEvent, py::call_guard<py::gil_scoped_release>())
        .def("listen_to_custom_event", &events::binding::EventListenerBinding::listenToCustomEvent, py::call_guard<py::gil_scoped_release>())
        .def("unlisten_to_default_event", &events::binding::EventListenerBinding::unlistenToDefaultEvent, py::call_guard<py::gil_scoped_release>())
        .def("unlisten_to_custom_event", &events::binding::EventListenerBinding::unlistenToCustomEvent, py::call_guard<py::gil_scoped_release>())
        .def("is_listening_to_default_event", &events::binding::EventListenerBinding::isListeningToDefaultEvent,
             py::call_guard<py::gil_scoped_release>())
        .def("is_listening_to_custom_event", &events::binding::EventListenerBinding::isListeningToCustomEvent,
             py::call_guard<py::gil_scoped_release>())
        .def("unlisten_all", &events::binding::EventListenerBinding::unlistenAll, py::call_guard<py::gil_scoped_release>());

    auto plugins_module = m.def_submodule("plugins");

    auto server_module = m.def_submodule("server");
    py::class_<server::ServerOutputInfoTimeStamp>(server_module, "ServerOutputInfoTimeStamp")
        .def(py::init<int, int, int>(), py::call_guard<py::gil_scoped_release>())
        .def_readwrite("hour", &server::ServerOutputInfoTimeStamp::hour)
        .def_readwrite("minute", &server::ServerOutputInfoTimeStamp::minute)
        .def_readwrite("second", &server::ServerOutputInfoTimeStamp::second);
    py::class_<server::PreprocessedInfo>(server_module, "PreprocessedInfo")
        .def(py::init<server::ServerOutputInfoTimeStamp, std::string>(), py::call_guard<py::gil_scoped_release>())
        .def_readwrite("timestamp", &server::PreprocessedInfo::timestamp)
        .def_readwrite("log_level", &server::PreprocessedInfo::log_level);
    py::class_<server::ServerOutputInfo>(server_module, "ServerOutputInfo")
        .def(py::init<server::ServerOutputInfoTimeStamp, std::string, std::string, std::string, std::optional<std::string>>(), py::arg("timestamp"),
             py::arg("raw_content"), py::arg("content"), py::arg("log_level"), py::arg("player_name") = py::none{},
             py::call_guard<py::gil_scoped_release>())
        .def_readwrite("id", &server::ServerOutputInfo::id)
        .def_readwrite("preprocessed_info", &server::ServerOutputInfo::preprocessed_info)
        .def_readwrite("raw_content", &server::ServerOutputInfo::raw_content)
        .def_readwrite("content", &server::ServerOutputInfo::content)
        .def_readwrite("player_name", &server::ServerOutputInfo::player_name);

    py::class_<server::binding::ServerInstanceBindingHelper>(server_module, "ServerInstance")
        .def(py::init<>(), py::call_guard<py::gil_scoped_release>());

    py::class_<server::binding::ArclightServerOutputParserBindingHelper>(server_module, "ArclightServerOutputParser")
        .def(py::init<>(), py::call_guard<py::gil_scoped_release>())
        .def("get_parser_name", &server::binding::ArclightServerOutputParserBindingHelper::getParserName)
        .def("get_send_message_command", &server::binding::ArclightServerOutputParserBindingHelper::getSendMessageCommand)
        .def("get_broadcast_message_command", &server::binding::ArclightServerOutputParserBindingHelper::getBroadcastMessageCommand)
        .def("get_stop_command", &server::binding::ArclightServerOutputParserBindingHelper::getStopCommand)
        .def("preprocess_server_output", &server::binding::ArclightServerOutputParserBindingHelper::preprocessServerOutput)
        .def("parse_server_output", &server::binding::ArclightServerOutputParserBindingHelper::parseServerOutput)
        .def("parse_player_joined", &server::binding::ArclightServerOutputParserBindingHelper::parsePlayerJoined)
        .def("parse_player_left", &server::binding::ArclightServerOutputParserBindingHelper::parsePlayerLeft)
        .def("parse_server_version", &server::binding::ArclightServerOutputParserBindingHelper::parseServerVersion)
        .def("parse_server_address", &server::binding::ArclightServerOutputParserBindingHelper::parseServerAddress)
        .def("test_server_startup_done", &server::binding::ArclightServerOutputParserBindingHelper::testServerStartupDone)
        .def("test_rcon_started", &server::binding::ArclightServerOutputParserBindingHelper::testRCONStarted)
        .def("test_server_stopping", &server::binding::ArclightServerOutputParserBindingHelper::testServerStopping);
    py::class_<server::binding::Beta18ServerOutputParserBindingHelper>(server_module, "Beta18ServerOutputParser")
        .def(py::init<>(), py::call_guard<py::gil_scoped_release>())
        .def("get_parser_name", &server::binding::Beta18ServerOutputParserBindingHelper::getParserName)
        .def("get_send_message_command", &server::binding::Beta18ServerOutputParserBindingHelper::getSendMessageCommand)
        .def("get_broadcast_message_command", &server::binding::Beta18ServerOutputParserBindingHelper::getBroadcastMessageCommand)
        .def("get_stop_command", &server::binding::Beta18ServerOutputParserBindingHelper::getStopCommand)
        .def("preprocess_server_output", &server::binding::Beta18ServerOutputParserBindingHelper::preprocessServerOutput)
        .def("parse_server_output", &server::binding::Beta18ServerOutputParserBindingHelper::parseServerOutput)
        .def("parse_player_joined", &server::binding::Beta18ServerOutputParserBindingHelper::parsePlayerJoined)
        .def("parse_player_left", &server::binding::Beta18ServerOutputParserBindingHelper::parsePlayerLeft)
        .def("parse_server_version", &server::binding::Beta18ServerOutputParserBindingHelper::parseServerVersion)
        .def("parse_server_address", &server::binding::Beta18ServerOutputParserBindingHelper::parseServerAddress)
        .def("test_server_startup_done", &server::binding::Beta18ServerOutputParserBindingHelper::testServerStartupDone)
        .def("test_rcon_started", &server::binding::Beta18ServerOutputParserBindingHelper::testRCONStarted)
        .def("test_server_stopping", &server::binding::Beta18ServerOutputParserBindingHelper::testServerStopping);
    py::class_<server::binding::BukkitServerOutputParserBindingHelper>(server_module, "BukkitServerOutputParser")
        .def(py::init<>(), py::call_guard<py::gil_scoped_release>())
        .def("get_parser_name", &server::binding::BukkitServerOutputParserBindingHelper::getParserName)
        .def("get_send_message_command", &server::binding::BukkitServerOutputParserBindingHelper::getSendMessageCommand)
        .def("get_broadcast_message_command", &server::binding::BukkitServerOutputParserBindingHelper::getBroadcastMessageCommand)
        .def("get_stop_command", &server::binding::BukkitServerOutputParserBindingHelper::getStopCommand)
        .def("preprocess_server_output", &server::binding::BukkitServerOutputParserBindingHelper::preprocessServerOutput)
        .def("parse_server_output", &server::binding::BukkitServerOutputParserBindingHelper::parseServerOutput)
        .def("parse_player_joined", &server::binding::BukkitServerOutputParserBindingHelper::parsePlayerJoined)
        .def("parse_player_left", &server::binding::BukkitServerOutputParserBindingHelper::parsePlayerLeft)
        .def("parse_server_version", &server::binding::BukkitServerOutputParserBindingHelper::parseServerVersion)
        .def("parse_server_address", &server::binding::BukkitServerOutputParserBindingHelper::parseServerAddress)
        .def("test_server_startup_done", &server::binding::BukkitServerOutputParserBindingHelper::testServerStartupDone)
        .def("test_rcon_started", &server::binding::BukkitServerOutputParserBindingHelper::testRCONStarted)
        .def("test_server_stopping", &server::binding::BukkitServerOutputParserBindingHelper::testServerStopping);
    py::class_<server::binding::BukkitLegacyServerOutputParserBindingHelper>(server_module, "BukkitLegacyServerOutputParser")
        .def(py::init<>(), py::call_guard<py::gil_scoped_release>())
        .def("get_parser_name", &server::binding::BukkitLegacyServerOutputParserBindingHelper::getParserName)
        .def("get_send_message_command", &server::binding::BukkitLegacyServerOutputParserBindingHelper::getSendMessageCommand)
        .def("get_broadcast_message_command", &server::binding::BukkitLegacyServerOutputParserBindingHelper::getBroadcastMessageCommand)
        .def("get_stop_command", &server::binding::BukkitLegacyServerOutputParserBindingHelper::getStopCommand)
        .def("preprocess_server_output", &server::binding::BukkitLegacyServerOutputParserBindingHelper::preprocessServerOutput)
        .def("parse_server_output", &server::binding::BukkitLegacyServerOutputParserBindingHelper::parseServerOutput)
        .def("parse_player_joined", &server::binding::BukkitLegacyServerOutputParserBindingHelper::parsePlayerJoined)
        .def("parse_player_left", &server::binding::BukkitLegacyServerOutputParserBindingHelper::parsePlayerLeft)
        .def("parse_server_version", &server::binding::BukkitLegacyServerOutputParserBindingHelper::parseServerVersion)
        .def("parse_server_address", &server::binding::BukkitLegacyServerOutputParserBindingHelper::parseServerAddress)
        .def("test_server_startup_done", &server::binding::BukkitLegacyServerOutputParserBindingHelper::testServerStartupDone)
        .def("test_rcon_started", &server::binding::BukkitLegacyServerOutputParserBindingHelper::testRCONStarted)
        .def("test_server_stopping", &server::binding::BukkitLegacyServerOutputParserBindingHelper::testServerStopping);
    py::class_<server::binding::BungeecordServerOutputParserBindingHelper>(server_module, "BungeecordServerOutputParser")
        .def(py::init<>(), py::call_guard<py::gil_scoped_release>())
        .def("get_parser_name", &server::binding::BungeecordServerOutputParserBindingHelper::getParserName)
        .def("get_send_message_command", &server::binding::BungeecordServerOutputParserBindingHelper::getSendMessageCommand)
        .def("get_broadcast_message_command", &server::binding::BungeecordServerOutputParserBindingHelper::getBroadcastMessageCommand)
        .def("get_stop_command", &server::binding::BungeecordServerOutputParserBindingHelper::getStopCommand)
        .def("preprocess_server_output", &server::binding::BungeecordServerOutputParserBindingHelper::preprocessServerOutput)
        .def("parse_server_output", &server::binding::BungeecordServerOutputParserBindingHelper::parseServerOutput)
        .def("parse_player_joined", &server::binding::BungeecordServerOutputParserBindingHelper::parsePlayerJoined)
        .def("parse_player_left", &server::binding::BungeecordServerOutputParserBindingHelper::parsePlayerLeft)
        .def("parse_server_version", &server::binding::BungeecordServerOutputParserBindingHelper::parseServerVersion)
        .def("parse_server_address", &server::binding::BungeecordServerOutputParserBindingHelper::parseServerAddress)
        .def("test_server_startup_done", &server::binding::BungeecordServerOutputParserBindingHelper::testServerStartupDone)
        .def("test_rcon_started", &server::binding::BungeecordServerOutputParserBindingHelper::testRCONStarted)
        .def("test_server_stopping", &server::binding::BungeecordServerOutputParserBindingHelper::testServerStopping);
    py::class_<server::binding::CatServerOutputParserBindingHelper>(server_module, "CatServerOutputParser")
        .def(py::init<>(), py::call_guard<py::gil_scoped_release>())
        .def("get_parser_name", &server::binding::CatServerOutputParserBindingHelper::getParserName)
        .def("get_send_message_command", &server::binding::CatServerOutputParserBindingHelper::getSendMessageCommand)
        .def("get_broadcast_message_command", &server::binding::CatServerOutputParserBindingHelper::getBroadcastMessageCommand)
        .def("get_stop_command", &server::binding::CatServerOutputParserBindingHelper::getStopCommand)
        .def("preprocess_server_output", &server::binding::CatServerOutputParserBindingHelper::preprocessServerOutput)
        .def("parse_server_output", &server::binding::CatServerOutputParserBindingHelper::parseServerOutput)
        .def("parse_player_joined", &server::binding::CatServerOutputParserBindingHelper::parsePlayerJoined)
        .def("parse_player_left", &server::binding::CatServerOutputParserBindingHelper::parsePlayerLeft)
        .def("parse_server_version", &server::binding::CatServerOutputParserBindingHelper::parseServerVersion)
        .def("parse_server_address", &server::binding::CatServerOutputParserBindingHelper::parseServerAddress)
        .def("test_server_startup_done", &server::binding::CatServerOutputParserBindingHelper::testServerStartupDone)
        .def("test_rcon_started", &server::binding::CatServerOutputParserBindingHelper::testRCONStarted)
        .def("test_server_stopping", &server::binding::CatServerOutputParserBindingHelper::testServerStopping);
    py::class_<server::binding::ForgeServerOutputParserBindingHelper>(server_module, "ForgeServerOutputParser")
        .def(py::init<>(), py::call_guard<py::gil_scoped_release>())
        .def("get_parser_name", &server::binding::ForgeServerOutputParserBindingHelper::getParserName)
        .def("get_send_message_command", &server::binding::ForgeServerOutputParserBindingHelper::getSendMessageCommand)
        .def("get_broadcast_message_command", &server::binding::ForgeServerOutputParserBindingHelper::getBroadcastMessageCommand)
        .def("get_stop_command", &server::binding::ForgeServerOutputParserBindingHelper::getStopCommand)
        .def("preprocess_server_output", &server::binding::ForgeServerOutputParserBindingHelper::preprocessServerOutput)
        .def("parse_server_output", &server::binding::ForgeServerOutputParserBindingHelper::parseServerOutput)
        .def("parse_player_joined", &server::binding::ForgeServerOutputParserBindingHelper::parsePlayerJoined)
        .def("parse_player_left", &server::binding::ForgeServerOutputParserBindingHelper::parsePlayerLeft)
        .def("parse_server_version", &server::binding::ForgeServerOutputParserBindingHelper::parseServerVersion)
        .def("parse_server_address", &server::binding::ForgeServerOutputParserBindingHelper::parseServerAddress)
        .def("test_server_startup_done", &server::binding::ForgeServerOutputParserBindingHelper::testServerStartupDone)
        .def("test_rcon_started", &server::binding::ForgeServerOutputParserBindingHelper::testRCONStarted)
        .def("test_server_stopping", &server::binding::ForgeServerOutputParserBindingHelper::testServerStopping);
    py::class_<server::binding::PythonCustomParserWrapperBindingHelper>(server_module, "PythonCustomParserWrapper")
        .def(py::init<>(), py::call_guard<py::gil_scoped_release>())
        .def("get_parser_name", &server::binding::PythonCustomParserWrapperBindingHelper::getParserName)
        .def("get_send_message_command", &server::binding::PythonCustomParserWrapperBindingHelper::getSendMessageCommand)
        .def("get_broadcast_message_command", &server::binding::PythonCustomParserWrapperBindingHelper::getBroadcastMessageCommand)
        .def("get_stop_command", &server::binding::PythonCustomParserWrapperBindingHelper::getStopCommand)
        .def("preprocess_server_output", &server::binding::PythonCustomParserWrapperBindingHelper::preprocessServerOutput)
        .def("parse_server_output", &server::binding::PythonCustomParserWrapperBindingHelper::parseServerOutput)
        .def("parse_player_joined", &server::binding::PythonCustomParserWrapperBindingHelper::parsePlayerJoined)
        .def("parse_player_left", &server::binding::PythonCustomParserWrapperBindingHelper::parsePlayerLeft)
        .def("parse_server_version", &server::binding::PythonCustomParserWrapperBindingHelper::parseServerVersion)
        .def("parse_server_address", &server::binding::PythonCustomParserWrapperBindingHelper::parseServerAddress)
        .def("test_server_startup_done", &server::binding::PythonCustomParserWrapperBindingHelper::testServerStartupDone)
        .def("test_rcon_started", &server::binding::PythonCustomParserWrapperBindingHelper::testRCONStarted)
        .def("test_server_stopping", &server::binding::PythonCustomParserWrapperBindingHelper::testServerStopping);
    py::class_<server::binding::VanillaServerOutputParserBindingHelper>(server_module, "VanillaServerOutputParser")
        .def(py::init<>(), py::call_guard<py::gil_scoped_release>())
        .def("get_parser_name", &server::binding::VanillaServerOutputParserBindingHelper::getParserName)
        .def("get_send_message_command", &server::binding::VanillaServerOutputParserBindingHelper::getSendMessageCommand)
        .def("get_broadcast_message_command", &server::binding::VanillaServerOutputParserBindingHelper::getBroadcastMessageCommand)
        .def("get_stop_command", &server::binding::VanillaServerOutputParserBindingHelper::getStopCommand)
        .def("preprocess_server_output", &server::binding::VanillaServerOutputParserBindingHelper::preprocessServerOutput)
        .def("parse_server_output", &server::binding::VanillaServerOutputParserBindingHelper::parseServerOutput)
        .def("parse_player_joined", &server::binding::VanillaServerOutputParserBindingHelper::parsePlayerJoined)
        .def("parse_player_left", &server::binding::VanillaServerOutputParserBindingHelper::parsePlayerLeft)
        .def("parse_server_version", &server::binding::VanillaServerOutputParserBindingHelper::parseServerVersion)
        .def("parse_server_address", &server::binding::VanillaServerOutputParserBindingHelper::parseServerAddress)
        .def("test_server_startup_done", &server::binding::VanillaServerOutputParserBindingHelper::testServerStartupDone)
        .def("test_rcon_started", &server::binding::VanillaServerOutputParserBindingHelper::testRCONStarted)
        .def("test_server_stopping", &server::binding::VanillaServerOutputParserBindingHelper::testServerStopping);
    py::class_<server::binding::VelocityServerOutputParserBindingHelper>(server_module, "VelocityServerOutputParser")
        .def(py::init<>(), py::call_guard<py::gil_scoped_release>())
        .def("get_parser_name", &server::binding::VelocityServerOutputParserBindingHelper::getParserName)
        .def("get_send_message_command", &server::binding::VelocityServerOutputParserBindingHelper::getSendMessageCommand)
        .def("get_broadcast_message_command", &server::binding::VelocityServerOutputParserBindingHelper::getBroadcastMessageCommand)
        .def("get_stop_command", &server::binding::VelocityServerOutputParserBindingHelper::getStopCommand)
        .def("preprocess_server_output", &server::binding::VelocityServerOutputParserBindingHelper::preprocessServerOutput)
        .def("parse_server_output", &server::binding::VelocityServerOutputParserBindingHelper::parseServerOutput)
        .def("parse_player_joined", &server::binding::VelocityServerOutputParserBindingHelper::parsePlayerJoined)
        .def("parse_player_left", &server::binding::VelocityServerOutputParserBindingHelper::parsePlayerLeft)
        .def("parse_server_version", &server::binding::VelocityServerOutputParserBindingHelper::parseServerVersion)
        .def("parse_server_address", &server::binding::VelocityServerOutputParserBindingHelper::parseServerAddress)
        .def("test_server_startup_done", &server::binding::VelocityServerOutputParserBindingHelper::testServerStartupDone)
        .def("test_rcon_started", &server::binding::VelocityServerOutputParserBindingHelper::testRCONStarted)
        .def("test_server_stopping", &server::binding::VelocityServerOutputParserBindingHelper::testServerStopping);
    py::class_<server::binding::WaterfallServerOutputParserBindingHelper>(server_module, "WaterfallServerOutputParser")
        .def(py::init<>(), py::call_guard<py::gil_scoped_release>())
        .def("get_parser_name", &server::binding::WaterfallServerOutputParserBindingHelper::getParserName)
        .def("get_send_message_command", &server::binding::WaterfallServerOutputParserBindingHelper::getSendMessageCommand)
        .def("get_broadcast_message_command", &server::binding::WaterfallServerOutputParserBindingHelper::getBroadcastMessageCommand)
        .def("get_stop_command", &server::binding::WaterfallServerOutputParserBindingHelper::getStopCommand)
        .def("preprocess_server_output", &server::binding::WaterfallServerOutputParserBindingHelper::preprocessServerOutput)
        .def("parse_server_output", &server::binding::WaterfallServerOutputParserBindingHelper::parseServerOutput)
        .def("parse_player_joined", &server::binding::WaterfallServerOutputParserBindingHelper::parsePlayerJoined)
        .def("parse_player_left", &server::binding::WaterfallServerOutputParserBindingHelper::parsePlayerLeft)
        .def("parse_server_version", &server::binding::WaterfallServerOutputParserBindingHelper::parseServerVersion)
        .def("parse_server_address", &server::binding::WaterfallServerOutputParserBindingHelper::parseServerAddress)
        .def("test_server_startup_done", &server::binding::WaterfallServerOutputParserBindingHelper::testServerStartupDone)
        .def("test_rcon_started", &server::binding::WaterfallServerOutputParserBindingHelper::testRCONStarted)
        .def("test_server_stopping", &server::binding::WaterfallServerOutputParserBindingHelper::testServerStopping);

    auto utils_module = m.def_submodule("utils");

    auto rtext_module = utils_module.def_submodule("rtext");
    py::class_<utils::rtext::RStyleClassic>(rtext_module, "RStyleClassic")
        .def_readonly_static("bold", &utils::rtext::RStyleClassic::bold)
        .def_readonly_static("italic", &utils::rtext::RStyleClassic::italic)
        .def_readonly_static("underlined", &utils::rtext::RStyleClassic::underlined)
        .def_readonly_static("strikethrough", &utils::rtext::RStyleClassic::strikethrough)
        .def_readonly_static("obfuscated", &utils::rtext::RStyleClassic::obfuscated);
    py::class_<utils::rtext::RColorClassic>(rtext_module, "RColorClassic")
        .def_readonly_static("black", &utils::rtext::RColorClassic::black)
        .def_readonly_static("dark_blue", &utils::rtext::RColorClassic::dark_blue)
        .def_readonly_static("dark_green", &utils::rtext::RColorClassic::dark_green)
        .def_readonly_static("dark_aqua", &utils::rtext::RColorClassic::dark_aqua)
        .def_readonly_static("dark_red", &utils::rtext::RColorClassic::dark_red)
        .def_readonly_static("dark_purple", &utils::rtext::RColorClassic::dark_purple)
        .def_readonly_static("gold", &utils::rtext::RColorClassic::gold)
        .def_readonly_static("gray", &utils::rtext::RColorClassic::gray)
        .def_readonly_static("dark_gray", &utils::rtext::RColorClassic::dark_gray)
        .def_readonly_static("blue", &utils::rtext::RColorClassic::blue)
        .def_readonly_static("green", &utils::rtext::RColorClassic::green)
        .def_readonly_static("aqua", &utils::rtext::RColorClassic::aqua)
        .def_readonly_static("red", &utils::rtext::RColorClassic::red)
        .def_readonly_static("light_purple", &utils::rtext::RColorClassic::light_purple)
        .def_readonly_static("yellow", &utils::rtext::RColorClassic::yellow)
        .def_readonly_static("white", &utils::rtext::RColorClassic::white)
        .def_readonly_static("reset", &utils::rtext::RColorClassic::reset);
    py::class_<utils::rtext::RColor>(rtext_module, "RColor")
        .def(py::init<short const, short const, short const>(), py::arg("red"), py::arg("green"), py::arg("blue"),
             py::call_guard<py::gil_scoped_release>())
        .def_property("red", &utils::rtext::RColor::getRed, &utils::rtext::RColor::setRed)
        .def_property("green", &utils::rtext::RColor::getGreen, &utils::rtext::RColor::setGreen)
        .def_property("blue", &utils::rtext::RColor::getBlue, &utils::rtext::RColor::setBlue)
        .def("to_string", &utils::rtext::RColor::toString);
    py::class_<utils::rtext::RAction>(rtext_module, "RAction")
        .def_readonly_static("suggest_command", &utils::rtext::RAction::suggest_command)
        .def_readonly_static("run_command", &utils::rtext::RAction::run_command)
        .def_readonly_static("open_url", &utils::rtext::RAction::open_url)
        .def_readonly_static("open_file", &utils::rtext::RAction::open_file)
        .def_readonly_static("change_page", &utils::rtext::RAction::change_page)
        .def_readonly_static("copy_to_clipboard", &utils::rtext::RAction::copy_to_clipboard);
    py::class_<utils::rtext::RText>(rtext_module, "RText")
        .def(py::init<>(), py::call_guard<py::gil_scoped_release>())
        .def(py::init<std::string const &>(), py::call_guard<py::gil_scoped_release>())
        .def("to_json_string", &utils::rtext::RText::toJSONString, py::call_guard<py::gil_scoped_release>())
        .def("to_plain_text", &utils::rtext::RText::toPlainText, py::call_guard<py::gil_scoped_release>())
        .def("to_colored_text", &utils::rtext::RText::toColoredText, py::call_guard<py::gil_scoped_release>())
        .def("to_legacy_text", &utils::rtext::RText::toLegacyText, py::call_guard<py::gil_scoped_release>())
        .def("set_text", static_cast<utils::rtext::RText &(utils::rtext::RText::*)(std::string const &)>(&utils::rtext::RText::setText))
        .def("set_text",
             static_cast<utils::rtext::RText &(utils::rtext::RText::*)(std::initializer_list<std::string>)>(&utils::rtext::RText::setText))
        .def("set_text", &utils::rtext::RText::setText<std::vector<std::string>>)
        .def("set_font", &utils::rtext::RText::setFont)
        .def("set_color", py::overload_cast<utils::rtext::RColorClassic::RColorClassicInternal const &>(&utils::rtext::RText::setColor))
        .def("set_color", py::overload_cast<utils::rtext::RColor const &>(&utils::rtext::RText::setColor))
        .def("set_style", static_cast<utils::rtext::RText &(utils::rtext::RText::*)(utils::rtext::RStyleClassic::RStyleClassicInternal const &)>(
                              &utils::rtext::RText::setStyle))
        .def("set_style",
             static_cast<utils::rtext::RText &(utils::rtext::RText::*)(std::initializer_list<utils::rtext::RStyleClassic::RStyleClassicInternal>)>(
                 &utils::rtext::RText::setStyle))
        .def("set_style", &utils::rtext::RText::setStyle<std::vector<utils::rtext::RStyleClassic::RStyleClassicInternal>>)
        .def("set_click_event", &utils::rtext::RText::setClickEvent)
        .def("set_hover_text", py::overload_cast<std::string const &>(&utils::rtext::RText::setHoverText))
        .def("set_hover_text", py::overload_cast<utils::rtext::RText const &>(&utils::rtext::RText::setHoverText));
}