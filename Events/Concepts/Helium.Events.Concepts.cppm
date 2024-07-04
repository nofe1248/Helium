/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <concepts>
#include <string>
#include <type_traits>

export module Helium.Events.Concepts;

export namespace helium::events::concepts {
    template<typename EventType>
    concept IsEvent = std::movable<EventType> and std::copyable<EventType> and requires {
        { EventType::getName() } -> std::convertible_to<std::string>;
    };

    template<typename EventType>
    concept IsCancellableEvent = IsEvent<EventType> and requires(EventType event_ins, bool b) {
        { event_ins.isCancelled() } -> std::same_as<bool>;
        { event_ins.cancel() } -> std::same_as<void>;
        { event_ins.uncancel() } -> std::same_as<void>;
        { event_ins.setCancelled(b) } -> std::same_as<void>;
    };

    template<typename EventType, typename ListenerType>
    concept IsEventListener = std::movable<ListenerType> and std::copyable<ListenerType> and
                              requires(EventType event_ins, ListenerType listener_ins) {
                                  { listener_ins.handle(event_ins) } -> std::same_as<void>;
                                { listener_ins.getPriority() } -> std::same_as<std::size_t>;
                            };

    template<typename EventEmitterMixinType>
    concept IsEventEmitterPolicy = true;

    template<typename EventEmitterMixinType>
    concept IsEventListenerPolicy = true;

    template<typename Policy>
    concept IsPolicy = IsEventEmitterPolicy<Policy> or IsEventListenerPolicy<Policy>;
} // namespace helium::events
