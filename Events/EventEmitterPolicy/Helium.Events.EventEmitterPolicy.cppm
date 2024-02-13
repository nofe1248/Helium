/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <concepts>

#include <boost/mp11.hpp>

export module Helium.Events.EventEmitterPolicy;

import Helium.Events.Concepts;

export namespace helium::events {
    template<template<typename> class... Mixins_>
    struct EventEmitterMixinList {};
} // namespace helium::events

export namespace helium::events::details {
    template<EventEmitterPolicy Policy>
    consteval auto hasTypeMixins() -> bool {
        if constexpr (requires { typename Policy::Mixins; }) {
            return true;
        }
        return false;
    }

    template<EventEmitterPolicy Policy, bool>
    struct SelectMixins {
        using Type = typename Policy::Mixins;
    };
    template<EventEmitterPolicy Policy>
    struct SelectMixins<Policy, false> {
        using Type = EventEmitterMixinList<>;
    };

    template<typename Base, typename MixinList>
    struct InheritFromMixins;

    template<typename Base, template<typename> class T, template<typename> class... Args>
    struct InheritFromMixins<Base, EventEmitterMixinList<T, Args...>> {
        using Type = T<typename InheritFromMixins<Base, EventEmitterMixinList<Args...>>::Type>;
    };

    template<typename Base>
    struct InheritFromMixins<Base, EventEmitterMixinList<>> {
        using Type = Base;
    };
} // namespace helium::events::details
