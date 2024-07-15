/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <memory>

#include <cpptrace/cpptrace.hpp>

#include <pybind11/chrono.h>
#include <pybind11/embed.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>

export module Helium.Events.BindingHelper;

import Helium.Events.EventBus;
import Helium.Events.EventListener;
import Helium.Events.EventEmitter;
import Helium.Events.Helium;
import Helium.Logger;

namespace py = pybind11;

namespace helium::events::binding
{
auto binding_logger = logger::SharedLogger::getSharedLogger("Events", "PythonBinding");
}

export namespace helium::events::binding
{
enum class HeliumDefaultEventsBindingEnum
{
    HELIUM_STARTING,
    HELIUM_STARTED,
    HELIUM_STOPPING,
    PLUGIN_LOADED,
    PLUGIN_UNLOADED,
    PLUGIN_RELOADED,
    SERVER_STARTING,
    SERVER_STARTED,
    SERVER_STOPPING,
    SERVER_STOPPED,
    SERVER_PAUSED,
    SERVER_RESUMED,
    CONSOLE_INPUT,
    SERVER_OUTPUT_RAW,
    PLAYER_INPUT_RAW,
    SERVER_OUTPUT,
    PLAYER_INPUT,
};
class EventBusBinding final
{
private:
    std::shared_ptr<EventBus> bus_ptr_;

public:
    EventBusBinding() : bus_ptr_(std::make_shared<EventBus>())
    {
    }

    EventBusBinding(EventBusBinding const &) = default;
    EventBusBinding(EventBusBinding &&) noexcept = default;
    auto operator=(EventBusBinding const &) -> EventBusBinding & = default;
    auto operator=(EventBusBinding &&) noexcept -> EventBusBinding & = default;

    auto getEventBus() const noexcept -> std::shared_ptr<EventBus>
    {
        return this->bus_ptr_;
    }

    auto unlistenAll(EventListenerIDType const &listener_id) -> void
    {
        this->bus_ptr_->unlistenAll(listener_id);
    }

    auto processEvents() -> void
    {
        bus_ptr_->processEvents();
    }
};
class EventEmitterBinding
{
private:
    EventEmitter event_emitter_;

public:
    EventEmitterBinding() : event_emitter_(EventBus::getHeliumEventBus())
    {
    }
    EventEmitterBinding(EventBusBinding const &event_bus) : event_emitter_(event_bus.getEventBus())
    {
    }

    EventEmitterBinding(EventEmitterBinding const &) = default;
    EventEmitterBinding(EventEmitterBinding &&) noexcept = default;
    auto operator=(EventEmitterBinding const &) -> EventEmitterBinding & = default;
    auto operator=(EventEmitterBinding &&) noexcept -> EventEmitterBinding & = default;

    auto postponeDefaultEvent(HeliumDefaultEventsBindingEnum event_type, py::object event_arg = py::none{})
    {
        if (event_type == HeliumDefaultEventsBindingEnum::HELIUM_STARTING)
        {
            this->event_emitter_.postponeEvent(HeliumStarting{});
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::HELIUM_STARTED)
        {
            this->event_emitter_.postponeEvent(HeliumStarted{});
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::HELIUM_STOPPING)
        {
            this->event_emitter_.postponeEvent(HeliumStopping{});
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLUGIN_LOADED)
        {
            this->event_emitter_.postponeEvent(PluginLoaded{});
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLUGIN_UNLOADED)
        {
            this->event_emitter_.postponeEvent(PluginUnloaded{});
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLUGIN_RELOADED)
        {
            this->event_emitter_.postponeEvent(PluginReloaded{});
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STARTING)
        {
            this->event_emitter_.postponeEvent(ServerStarting{});
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STARTED)
        {
            this->event_emitter_.postponeEvent(ServerStarted{});
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STOPPING)
        {
            this->event_emitter_.postponeEvent(ServerStopping{});
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STOPPED)
        {
            this->event_emitter_.postponeEvent(ServerStopped{});
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_PAUSED)
        {
            this->event_emitter_.postponeEvent(ServerPaused{});
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_RESUMED)
        {
            this->event_emitter_.postponeEvent(ServerResumed{});
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::CONSOLE_INPUT)
        {
            this->event_emitter_.postponeEvent(ConsoleInput{event_arg.cast<std::string>()});
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_OUTPUT_RAW)
        {
            this->event_emitter_.postponeEvent(ServerOutputRaw{event_arg.cast<std::string>()});
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLAYER_INPUT_RAW)
        {
            this->event_emitter_.postponeEvent(PlayerInputRaw{event_arg.cast<std::string>()});
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_OUTPUT)
        {
            this->event_emitter_.postponeEvent(ServerOutput{});
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLAYER_INPUT)
        {
            this->event_emitter_.postponeEvent(PlayerInput{});
        }
        else
        {
            throw cpptrace::runtime_error("Unknown default event type");
        }
    }
    auto postponeCustomEvent(std::string const &event_id, py::object const &event_arg) -> void
    {
        this->event_emitter_.postponeDynamicIDEvent(PythonEvent{event_id, event_arg});
    }
};
class EventListenerBinding
{
private:
    EventListener event_listener_;

public:
    EventListenerBinding() : event_listener_(EventBus::getHeliumEventBus())
    {
    }
    EventListenerBinding(EventBusBinding const &event_bus) : event_listener_(event_bus.getEventBus())
    {
    }

    EventListenerBinding(EventListenerBinding const &) = delete;
    EventListenerBinding(EventListenerBinding &&) noexcept = default;
    auto operator=(EventListenerBinding const &) -> EventListenerBinding & = delete;
    auto operator=(EventListenerBinding &&) noexcept -> EventListenerBinding & = default;

    auto listenToDefaultEvent(HeliumDefaultEventsBindingEnum event_type, std::function<void(py::object)> const &callback) -> bool
    {
        binding_logger->debug("Listening to default event");
        if (event_type == HeliumDefaultEventsBindingEnum::HELIUM_STARTING)
        {
            return this->event_listener_.listenToEvent<HeliumStarting>(
                std::move([callback](HeliumStarting const &event) { callback(py::cast(event)); }));
        }
        if (event_type == HeliumDefaultEventsBindingEnum::HELIUM_STARTED)
        {
            binding_logger->debug("Registering");
            return this->event_listener_.listenToEvent<HeliumStarted>([callback](HeliumStarted const &event) { callback(py::cast(event)); });
        }
        if (event_type == HeliumDefaultEventsBindingEnum::HELIUM_STOPPING)
        {
            return this->event_listener_.listenToEvent<HeliumStopping>(
                std::move([callback](HeliumStopping const &event) { callback(py::cast(event)); }));
        }
        if (event_type == HeliumDefaultEventsBindingEnum::PLUGIN_LOADED)
        {
            return this->event_listener_.listenToEvent<PluginLoaded>(std::move([callback](PluginLoaded const &event) { callback(py::cast(event)); }));
        }
        if (event_type == HeliumDefaultEventsBindingEnum::PLUGIN_UNLOADED)
        {
            return this->event_listener_.listenToEvent<PluginUnloaded>(
                std::move([callback](PluginUnloaded const &event) { callback(py::cast(event)); }));
        }
        if (event_type == HeliumDefaultEventsBindingEnum::PLUGIN_RELOADED)
        {
            return this->event_listener_.listenToEvent<PluginReloaded>(
                std::move([callback](PluginReloaded const &event) { callback(py::cast(event)); }));
        }
        if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STARTING)
        {
            return this->event_listener_.listenToEvent<ServerStarting>(
                std::move([callback](ServerStarting const &event) { callback(py::cast(event)); }));
        }
        if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STARTED)
        {
            return this->event_listener_.listenToEvent<ServerStarted>(
                std::move([callback](ServerStarted const &event) { callback(py::cast(event)); }));
        }
        if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STOPPING)
        {
            return this->event_listener_.listenToEvent<ServerStopping>(
                std::move([callback](ServerStopping const &event) { callback(py::cast(event)); }));
        }
        if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STOPPED)
        {
            return this->event_listener_.listenToEvent<ServerStopped>(
                std::move([callback](ServerStopped const &event) { callback(py::cast(event)); }));
        }
        if (event_type == HeliumDefaultEventsBindingEnum::SERVER_PAUSED)
        {
            return this->event_listener_.listenToEvent<ServerPaused>(std::move([callback](ServerPaused const &event) { callback(py::cast(event)); }));
        }
        if (event_type == HeliumDefaultEventsBindingEnum::SERVER_RESUMED)
        {
            return this->event_listener_.listenToEvent<ServerResumed>(
                std::move([callback](ServerResumed const &event) { callback(py::cast(event)); }));
        }
        if (event_type == HeliumDefaultEventsBindingEnum::CONSOLE_INPUT)
        {
            return this->event_listener_.listenToEvent<ConsoleInput>(std::move([callback](ConsoleInput const &event) { callback(py::cast(event)); }));
        }
        if (event_type == HeliumDefaultEventsBindingEnum::SERVER_OUTPUT_RAW)
        {
            return this->event_listener_.listenToEvent<ServerOutputRaw>(
                std::move([callback](ServerOutputRaw const &event) { callback(py::cast(event)); }));
        }
        if (event_type == HeliumDefaultEventsBindingEnum::PLAYER_INPUT_RAW)
        {
            return this->event_listener_.listenToEvent<PlayerInputRaw>(
                std::move([callback](PlayerInputRaw const &event) { callback(py::cast(event)); }));
        }
        if (event_type == HeliumDefaultEventsBindingEnum::SERVER_OUTPUT)
        {
            return this->event_listener_.listenToEvent<ServerOutput>(std::move([callback](ServerOutput const &event) { callback(py::cast(event)); }));
        }
        if (event_type == HeliumDefaultEventsBindingEnum::PLAYER_INPUT)
        {
            return this->event_listener_.listenToEvent<PlayerInput>(std::move([callback](PlayerInput const &event) { callback(py::cast(event)); }));
        }
        throw cpptrace::runtime_error{"Unknown default event type"};
    }

    auto listenToCustomEvent(std::string const &event_id, std::function<void(PythonEvent const &)> const &callback) -> bool
    {
        return this->event_listener_.listenToDynamicIDEvent<PythonEvent>(event_id,
                                                                         std::move([callback](PythonEvent const &event) { callback(event); }));
    }

    auto unlistenToDefaultEvent(HeliumDefaultEventsBindingEnum event_type) -> void
    {
        if (event_type == HeliumDefaultEventsBindingEnum::HELIUM_STARTING)
        {
            this->event_listener_.unlistenToEvent<HeliumStarting>();
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::HELIUM_STARTED)
        {
            this->event_listener_.unlistenToEvent<HeliumStarted>();
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::HELIUM_STOPPING)
        {
            this->event_listener_.unlistenToEvent<HeliumStopping>();
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLUGIN_LOADED)
        {
            this->event_listener_.unlistenToEvent<PluginLoaded>();
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLUGIN_UNLOADED)
        {
            this->event_listener_.unlistenToEvent<PluginUnloaded>();
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLUGIN_RELOADED)
        {
            this->event_listener_.unlistenToEvent<PluginReloaded>();
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STARTING)
        {
            this->event_listener_.unlistenToEvent<ServerStarting>();
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STARTED)
        {
            this->event_listener_.unlistenToEvent<ServerStarted>();
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STOPPING)
        {
            this->event_listener_.unlistenToEvent<ServerStopping>();
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STOPPED)
        {
            this->event_listener_.unlistenToEvent<ServerStopped>();
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_PAUSED)
        {
            this->event_listener_.unlistenToEvent<ServerPaused>();
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_RESUMED)
        {
            this->event_listener_.unlistenToEvent<ServerResumed>();
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::CONSOLE_INPUT)
        {
            this->event_listener_.unlistenToEvent<ConsoleInput>();
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_OUTPUT_RAW)
        {
            this->event_listener_.unlistenToEvent<ServerOutputRaw>();
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLAYER_INPUT_RAW)
        {
            this->event_listener_.unlistenToEvent<PlayerInputRaw>();
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_OUTPUT)
        {
            this->event_listener_.unlistenToEvent<ServerOutput>();
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLAYER_INPUT)
        {
            this->event_listener_.unlistenToEvent<PlayerInput>();
        }
        else
        {
            throw cpptrace::runtime_error{"Unknown default event type"};
        }
    }

    auto unlistenToCustomEvent(std::string const &event_id) -> void
    {
        this->event_listener_.unlistenToDynamicIDEvent<PythonEvent>(event_id);
    }

    auto isListeningToDefaultEvent(HeliumDefaultEventsBindingEnum event_type) -> bool
    {
        if (event_type == HeliumDefaultEventsBindingEnum::HELIUM_STARTING)
        {
            return this->event_listener_.isListeningToEvent<HeliumStarting>();
        }
        if (event_type == HeliumDefaultEventsBindingEnum::HELIUM_STARTED)
        {
            return this->event_listener_.isListeningToEvent<HeliumStarted>();
        }
        if (event_type == HeliumDefaultEventsBindingEnum::HELIUM_STOPPING)
        {
            return this->event_listener_.isListeningToEvent<HeliumStopping>();
        }
        if (event_type == HeliumDefaultEventsBindingEnum::PLUGIN_LOADED)
        {
            return this->event_listener_.isListeningToEvent<PluginLoaded>();
        }
        if (event_type == HeliumDefaultEventsBindingEnum::PLUGIN_UNLOADED)
        {
            return this->event_listener_.isListeningToEvent<PluginUnloaded>();
        }
        if (event_type == HeliumDefaultEventsBindingEnum::PLUGIN_RELOADED)
        {
            return this->event_listener_.isListeningToEvent<PluginReloaded>();
        }
        if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STARTING)
        {
            return this->event_listener_.isListeningToEvent<ServerStarting>();
        }
        if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STARTED)
        {
            return this->event_listener_.isListeningToEvent<ServerStarted>();
        }
        if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STOPPING)
        {
            return this->event_listener_.isListeningToEvent<ServerStopping>();
        }
        if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STOPPED)
        {
            return this->event_listener_.isListeningToEvent<ServerStopped>();
        }
        if (event_type == HeliumDefaultEventsBindingEnum::SERVER_PAUSED)
        {
            return this->event_listener_.isListeningToEvent<ServerPaused>();
        }
        if (event_type == HeliumDefaultEventsBindingEnum::SERVER_RESUMED)
        {
            return this->event_listener_.isListeningToEvent<ServerResumed>();
        }
        if (event_type == HeliumDefaultEventsBindingEnum::CONSOLE_INPUT)
        {
            return this->event_listener_.isListeningToEvent<ConsoleInput>();
        }
        if (event_type == HeliumDefaultEventsBindingEnum::SERVER_OUTPUT_RAW)
        {
            return this->event_listener_.isListeningToEvent<ServerOutputRaw>();
        }
        if (event_type == HeliumDefaultEventsBindingEnum::PLAYER_INPUT_RAW)
        {
            return this->event_listener_.isListeningToEvent<PlayerInputRaw>();
        }
        if (event_type == HeliumDefaultEventsBindingEnum::SERVER_OUTPUT)
        {
            return this->event_listener_.isListeningToEvent<ServerOutput>();
        }
        if (event_type == HeliumDefaultEventsBindingEnum::PLAYER_INPUT)
        {
            return this->event_listener_.isListeningToEvent<PlayerInput>();
        }
        throw cpptrace::runtime_error{"Unknown default event type"};
    }

    auto isListeningToCustomEvent(std::string const &event_id) -> bool
    {
        return this->event_listener_.isListeningToDynamicIDEvent<PythonEvent>(event_id);
    }

    auto unlistenAll() -> void
    {
        this->event_listener_.unlistenAll();
    }
};
} // namespace helium::events::binding