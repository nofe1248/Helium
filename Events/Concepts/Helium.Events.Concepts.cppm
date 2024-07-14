/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <concepts>
#include <string>
#include <type_traits>

export module Helium.Events.Concepts;

export namespace helium::events::concepts
{
template <typename EventType>
concept IsEvent = std::movable<EventType> and std::copyable<EventType>;

template <typename EventType>
concept IsDynamicIDEvent = IsEvent<EventType> and requires(EventType event) {
    {
        event.event_id
    } -> std::convertible_to<std::string>;
};
} // namespace helium::events::concepts
