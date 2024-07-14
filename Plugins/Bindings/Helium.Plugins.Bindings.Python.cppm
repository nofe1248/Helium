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
    config_module.def("get_config", []() -> config::HeliumConfig & { return config::config; });

    auto command_module = m.def_submodule("commands");
    py::class_<commands::CommandSource>(command_module, "CommandSource")
        .def(py::init<std::string, std::string, std::optional<std::string>>(), py::arg("source_type"), py::arg("major_source"),
             py::arg("minor_source") = py::none{})
        .def(py::init<>());

    py::class_<commands::CommandContext>(command_module, "CommandContext").def(py::init<commands::CommandSource>());

    py::class_<commands::CommandNodeDescriptor, std::shared_ptr<commands::CommandNodeDescriptor>>(command_module, "CommandNodeDescriptor")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{});

    py::class_<commands::bindings::AbstractCommandNodeBinding>(command_module, "AbstractCommandNode")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{})
        .def("then", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::AbstractCommandNodeBinding::then))
        .def("then",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(&commands::bindings::AbstractCommandNodeBinding::then))
        .def("execute", &commands::bindings::AbstractCommandNodeBinding::execute)
        .def("require", &commands::bindings::AbstractCommandNodeBinding::require)
        .def("fork", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::AbstractCommandNodeBinding::fork))
        .def("fork",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(&commands::bindings::AbstractCommandNodeBinding::fork))
        .def("redirect",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::AbstractCommandNodeBinding::redirect))
        .def("redirect", py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                             &commands::bindings::AbstractCommandNodeBinding::redirect));

    py::class_<commands::bindings::CommandLiteralBaseBinding, commands::bindings::AbstractCommandNodeBinding>(command_module, "CommandLiteralBase")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{})
        .def("then", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandLiteralBaseBinding::then))
        .def("then",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(&commands::bindings::CommandLiteralBaseBinding::then))
        .def("execute", &commands::bindings::CommandLiteralBaseBinding::execute)
        .def("require", &commands::bindings::CommandLiteralBaseBinding::require)
        .def("fork", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandLiteralBaseBinding::fork))
        .def("fork",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(&commands::bindings::CommandLiteralBaseBinding::fork))
        .def("redirect",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandLiteralBaseBinding::redirect))
        .def("redirect", py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                             &commands::bindings::CommandLiteralBaseBinding::redirect));

    py::class_<commands::bindings::CommandLiteralStringBinding, commands::bindings::CommandLiteralBaseBinding>(command_module, "CommandLiteralString")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{})
        .def("then", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandLiteralStringBinding::then))
        .def("then",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(&commands::bindings::CommandLiteralStringBinding::then))
        .def("execute", &commands::bindings::CommandLiteralStringBinding::execute)
        .def("require", &commands::bindings::CommandLiteralStringBinding::require)
        .def("fork", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandLiteralStringBinding::fork))
        .def("fork",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(&commands::bindings::CommandLiteralStringBinding::fork))
        .def("redirect",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandLiteralStringBinding::redirect))
        .def("redirect", py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                             &commands::bindings::CommandLiteralStringBinding::redirect));

    py::class_<commands::bindings::CommandArgumentBaseBinding, commands::bindings::AbstractCommandNodeBinding>(command_module, "CommandArgumentBase")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{})
        .def("then", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentBaseBinding::then))
        .def("then",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(&commands::bindings::CommandArgumentBaseBinding::then))
        .def("execute", &commands::bindings::CommandArgumentBaseBinding::execute)
        .def("require", &commands::bindings::CommandArgumentBaseBinding::require)
        .def("fork", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentBaseBinding::fork))
        .def("fork",
             py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(&commands::bindings::CommandArgumentBaseBinding::fork))
        .def("redirect",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentBaseBinding::redirect))
        .def("redirect", py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                             &commands::bindings::CommandArgumentBaseBinding::redirect));

    py::class_<commands::bindings::CommandArgumentBooleanBinding, commands::bindings::CommandArgumentBaseBinding>(command_module,
                                                                                                                  "CommandArgumentBoolean")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{})
        .def("then", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentBooleanBinding::then))
        .def("then", py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                         &commands::bindings::CommandArgumentBooleanBinding::then))
        .def("execute", &commands::bindings::CommandArgumentBooleanBinding::execute)
        .def("require", &commands::bindings::CommandArgumentBooleanBinding::require)
        .def("fork", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentBooleanBinding::fork))
        .def("fork", py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                         &commands::bindings::CommandArgumentBooleanBinding::fork))
        .def("redirect",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentBooleanBinding::redirect))
        .def("redirect", py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                             &commands::bindings::CommandArgumentBooleanBinding::redirect));

    py::class_<commands::bindings::CommandArgumentIntegerBinding, commands::bindings::CommandArgumentBaseBinding>(command_module,
                                                                                                                  "CommandArgumentInteger")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{})
        .def("then", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentIntegerBinding::then))
        .def("then", py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                         &commands::bindings::CommandArgumentIntegerBinding::then))
        .def("execute", &commands::bindings::CommandArgumentIntegerBinding::execute)
        .def("require", &commands::bindings::CommandArgumentIntegerBinding::require)
        .def("fork", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentIntegerBinding::fork))
        .def("fork", py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                         &commands::bindings::CommandArgumentIntegerBinding::fork))
        .def("redirect",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentIntegerBinding::redirect))
        .def("redirect", py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                             &commands::bindings::CommandArgumentIntegerBinding::redirect));

    py::class_<commands::bindings::CommandArgumentFloatingPointBinding, commands::bindings::CommandArgumentBaseBinding>(
        command_module, "CommandArgumentFloatingPoint")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{})
        .def("then",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentFloatingPointBinding::then))
        .def("then", py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                         &commands::bindings::CommandArgumentFloatingPointBinding::then))
        .def("execute", &commands::bindings::CommandArgumentFloatingPointBinding::execute)
        .def("require", &commands::bindings::CommandArgumentFloatingPointBinding::require)
        .def("fork",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentFloatingPointBinding::fork))
        .def("fork", py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                         &commands::bindings::CommandArgumentFloatingPointBinding::fork))
        .def("redirect",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentFloatingPointBinding::redirect))
        .def("redirect", py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                             &commands::bindings::CommandArgumentFloatingPointBinding::redirect));

    py::class_<commands::bindings::CommandArgumentStringBinding, commands::bindings::CommandArgumentBaseBinding>(command_module,
                                                                                                                 "CommandArgumentString")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{})
        .def("then", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentStringBinding::then))
        .def("then", py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                         &commands::bindings::CommandArgumentStringBinding::then))
        .def("execute", &commands::bindings::CommandArgumentStringBinding::execute)
        .def("require", &commands::bindings::CommandArgumentStringBinding::require)
        .def("fork", py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentStringBinding::fork))
        .def("fork", py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                         &commands::bindings::CommandArgumentStringBinding::fork))
        .def("redirect",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentStringBinding::redirect))
        .def("redirect", py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                             &commands::bindings::CommandArgumentStringBinding::redirect));

    py::class_<commands::bindings::CommandArgumentQuotedStringBinding, commands::bindings::CommandArgumentBaseBinding>(command_module,
                                                                                                                       "CommandArgumentQuotedString")
        .def(py::init<std::string, std::optional<std::string>, std::optional<std::string>>(), py::arg("name"), py::arg("description") = py::none{},
             py::arg("abbreviated_name") = py::none{})
        .def("then",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentQuotedStringBinding::then))
        .def("then", py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                         &commands::bindings::CommandArgumentQuotedStringBinding::then))
        .def("execute", &commands::bindings::CommandArgumentQuotedStringBinding::execute)
        .def("require", &commands::bindings::CommandArgumentQuotedStringBinding::require)
        .def("fork",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentQuotedStringBinding::fork))
        .def("fork", py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                         &commands::bindings::CommandArgumentQuotedStringBinding::fork))
        .def("redirect",
             py::overload_cast<commands::bindings::AbstractCommandNodeBinding &>(&commands::bindings::CommandArgumentQuotedStringBinding::redirect))
        .def("redirect", py::overload_cast<std::vector<commands::bindings::AbstractCommandNodeBinding> &>(
                             &commands::bindings::CommandArgumentQuotedStringBinding::redirect));

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

    auto events_module = m.def_submodule("events");

    py::class_<events::HeliumStarting>(events_module, "HeliumStarting");
    py::class_<events::HeliumStarted>(events_module, "HeliumStarted");
    py::class_<events::HeliumStopping>(events_module, "HeliumStopping");
    py::class_<events::PluginLoaded>(events_module, "PluginLoaded");
    py::class_<events::PluginUnloaded>(events_module, "PluginUnloaded");
    py::class_<events::PluginReloaded>(events_module, "PluginReloaded");
    py::class_<events::ServerStarting>(events_module, "ServerStarting");
    py::class_<events::ServerStarted>(events_module, "ServerStarted");
    py::class_<events::ServerStopping>(events_module, "ServerStopping");
    py::class_<events::ServerStopped>(events_module, "ServerStopped");
    py::class_<events::ServerPaused>(events_module, "ServerPaused");
    py::class_<events::ServerResumed>(events_module, "ServerResumed");
    py::class_<events::ConsoleInput>(events_module, "ConsoleInput").def_readwrite("input", &events::ConsoleInput::input);
    py::class_<events::ServerOutputRaw>(events_module, "ServerOutputRaw").def_readwrite("output", &events::ServerOutputRaw::output);
    py::class_<events::PlayerInputRaw>(events_module, "PlayerInputRaw").def_readwrite("input", &events::PlayerInputRaw::input);
    py::class_<events::PythonEvent>(events_module, "PythonEvent").def_readwrite("event_arg", &events::PythonEvent::event_arg);

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
        .def("unlisten_all", &events::binding::EventBusBinding::unlistenAll)
        .def("process_events", &events::binding::EventBusBinding::processEvents);
    py::class_<events::binding::EventEmitterBinding>(events_module, "EventEmitter")
        .def(py::init<events::binding::EventBusBinding const &>())
        .def(py::init<>())
        .def("postpone_default_event", &events::binding::EventEmitterBinding::postponeDefaultEvent)
        .def("postpone_default_event", &events::binding::EventEmitterBinding::postponeDefaultEvent);
    py::class_<events::binding::EventListenerBinding>(events_module, "EventListener")
        .def(py::init<events::binding::EventBusBinding const &>())
        .def(py::init<>())
        .def("listen_to_default_event", &events::binding::EventListenerBinding::listenToDefaultEvent)
        .def("listen_to_custom_event", &events::binding::EventListenerBinding::listenToCustomEvent)
        .def("unlisten_to_default_event", &events::binding::EventListenerBinding::unlistenToDefaultEvent)
        .def("unlisten_to_custom_event", &events::binding::EventListenerBinding::unlistenToCustomEvent)
        .def("is_listening_to_default_event", &events::binding::EventListenerBinding::isListeningToDefaultEvent)
        .def("is_listening_to_custom_event", &events::binding::EventListenerBinding::isListeningToCustomEvent)
        .def("unlisten_all", &events::binding::EventListenerBinding::unlistenAll);

    auto plugins_module = m.def_submodule("plugins");

    auto servers_module = m.def_submodule("servers");
}