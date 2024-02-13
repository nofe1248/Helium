/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

export module Helium.Events.Policy.PolicyHelper;

import Helium.Events.Concepts;

export namespace helium::events {
    template<template<typename> class... Mixins_>
    struct MixinList {};
} // namespace helium::events

export namespace helium::events::details {
    template<typename Policy>
    concept hasTypeMixins = requires { typename Policy::Mixins; };

    template<concepts::IsPolicy Policy, bool>
    struct SelectMixins {
        using Type = typename Policy::Mixins;
    };
    template<concepts::IsPolicy Policy>
    struct SelectMixins<Policy, false> {
        using Type = MixinList<>;
    };

    template<typename Base, typename MixinList>
    struct InheritFromMixins;

    template<typename Base, template<typename> class T, template<typename> class... Args>
    struct InheritFromMixins<Base, MixinList<T, Args...>> {
        using Type = T<typename InheritFromMixins<Base, MixinList<Args...>>::Type>;
    };

    template<typename Base>
    struct InheritFromMixins<Base, MixinList<>> {
        using Type = Base;
    };
} // namespace helium::events::details

export namespace helium::events {
    template<typename Base_, typename Policy_>
    using BaseHelper = typename details::InheritFromMixins<
            Base_, typename details::SelectMixins<Policy_, details::hasTypeMixins<Policy_>>::Type>::Type;
}
