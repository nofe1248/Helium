/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <memory>

#include <pybind11/embed.h>

export module Helium.Events.BindingHelper;

import Helium.Events.EventBus;
import Helium.Events.EventListener;
import Helium.Events.EventEmitter;
import Helium.Events.Helium;

namespace py = pybind11;

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

    auto postponeDefaultEvent(HeliumDefaultEventsBindingEnum event_type, py::object event_arg = py::none{}) -> void
    {
    }

    auto postponeCustomEvent(std::string const &event_id, py::object const &event_arg) -> void
    {
        bus_ptr_->postponeEvent(PythonEvent{event_id, event_arg});
    }

    auto listenToDefaultEvent(HeliumDefaultEventsBindingEnum event_type, EventListenerIDType const &listener_id,
                              std::function<void(py::object const &)> &&callback) -> void
    {
    }

    auto listenToCustomEvent(std::string const &event_id, EventListenerIDType const &listener_id, std::function<void(PythonEvent const &)> &&callback)
        -> void
    {
    }

    auto unlistenToDefaultEvent(HeliumDefaultEventsBindingEnum event_type, EventListenerIDType const &listener_id) -> void
    {
    }

    auto unlistenToCustomEvent(std::string const &event_id, EventListenerIDType const &listener_id) -> void
    {
    }

    auto isListeningToDefaultEvent(HeliumDefaultEventsBindingEnum event_type, EventListenerIDType const &listener_id) -> void
    {
    }

    auto isListeningToCustomEvent(std::string const &event_id, EventListenerIDType const &listener_id) -> void
    {
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
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLUGIN_UNLOADED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLUGIN_RELOADED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STARTING)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STARTED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STOPPING)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STOPPED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_PAUSED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_RESUMED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::CONSOLE_INPUT)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_OUTPUT_RAW)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLAYER_INPUT_RAW)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_OUTPUT)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLAYER_INPUT)
        {
        }
    }
    auto postponeCustomEvent(std::string const &event_id, py::object const &event_arg) -> void
    {
    }
};
class EventListenerBinding
{
private:
    EventListener event_listener_;

public:
    EventListenerBinding(EventBusBinding const &event_bus) : event_listener_(event_bus.getEventBus())
    {
    }

    EventListenerBinding(EventListenerBinding const &) = delete;
    EventListenerBinding(EventListenerBinding &&) noexcept = default;
    auto operator=(EventListenerBinding const &) -> EventListenerBinding & = delete;
    auto operator=(EventListenerBinding &&) noexcept -> EventListenerBinding & = default;

    auto listenToDefaultEvent(HeliumDefaultEventsBindingEnum event_type, std::function<void(py::object const &)> &&callback) -> void
    {
        if (event_type == HeliumDefaultEventsBindingEnum::HELIUM_STARTING)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::HELIUM_STARTED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::HELIUM_STOPPING)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLUGIN_LOADED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLUGIN_UNLOADED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLUGIN_RELOADED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STARTING)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STARTED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STOPPING)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STOPPED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_PAUSED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_RESUMED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::CONSOLE_INPUT)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_OUTPUT_RAW)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLAYER_INPUT_RAW)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_OUTPUT)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLAYER_INPUT)
        {
        }
        throw std::runtime_error{"Unknown default event type"};
    }

    auto listenToCustomEvent(std::string const &event_id, std::function<void(PythonEvent const &)> &&callback) -> void
    {
    }

    auto unlistenToDefaultEvent(HeliumDefaultEventsBindingEnum event_type) -> void
    {
        if (event_type == HeliumDefaultEventsBindingEnum::HELIUM_STARTING)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::HELIUM_STARTED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::HELIUM_STOPPING)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLUGIN_LOADED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLUGIN_UNLOADED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLUGIN_RELOADED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STARTING)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STARTED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STOPPING)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STOPPED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_PAUSED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_RESUMED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::CONSOLE_INPUT)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_OUTPUT_RAW)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLAYER_INPUT_RAW)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_OUTPUT)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLAYER_INPUT)
        {
        }
        throw std::runtime_error{"Unknown default event type"};
    }

    auto unlistenToCustomEvent(std::string const &event_id) -> void
    {
    }

    auto isListeningToDefaultEvent(HeliumDefaultEventsBindingEnum event_type) -> void
    {
        if (event_type == HeliumDefaultEventsBindingEnum::HELIUM_STARTING)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::HELIUM_STARTED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::HELIUM_STOPPING)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLUGIN_LOADED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLUGIN_UNLOADED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLUGIN_RELOADED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STARTING)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STARTED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STOPPING)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_STOPPED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_PAUSED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_RESUMED)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::CONSOLE_INPUT)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_OUTPUT_RAW)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLAYER_INPUT_RAW)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::SERVER_OUTPUT)
        {
        }
        else if (event_type == HeliumDefaultEventsBindingEnum::PLAYER_INPUT)
        {
        }
        throw std::runtime_error{"Unknown default event type"};
    }

    auto isListeningToCustomEvent(std::string const &event_id) -> void
    {
    }

    auto unlistenAll() -> void
    {
        this->event_listener_.unlistenAll();
    }
};
} // namespace helium::events::binding