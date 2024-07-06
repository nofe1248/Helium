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
concept IsEvent = std::movable<EventType> and std::copyable<EventType>;
} // namespace helium::events
