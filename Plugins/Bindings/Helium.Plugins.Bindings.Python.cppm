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
import Helium.Events;
import Helium.Logger;

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(helium, m)
{
    using namespace helium;

    m.doc() = "Python bindings for Helium plugins";

    auto base_module = m.def_submodule("base");

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

    auto servers_module = m.def_submodule("servers");
}