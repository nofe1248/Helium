/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <algorithm>
#include <compare>
#include <concepts>
#include <string>
#include <type_traits>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(_WIN64)
#define BOOST_UUID_RANDOM_PROVIDER_FORCE_WINCRYPT
#endif

#include <boost/mp11.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

export module Helium.Events.EventEmitter;

import Helium.Base.HeliumObject;

import Helium.Events.Concepts;
import Helium.Events.EventEmitterPolicy;

export namespace helium::events::details {
    template<EventEmitterPolicy Policy_, typename MixinRoot_>
    class EventEmitterBase {
    protected:
        using ThisType = EventEmitterBase<Policy_, MixinRoot_>;

        using MixinRoot = std::conditional_t<std::same_as<MixinRoot_, void>, ThisType, MixinRoot_>;

        using Policy = Policy_;

        using Mixins = typename SelectMixins<Policy, details::hasTypeMixins<Policy>()>::Type;

    public:
        EventEmitterBase() = default;
        EventEmitterBase(EventEmitterBase const &that) = default;
        EventEmitterBase(EventEmitterBase &&that) noexcept = default;
        auto operator=(EventEmitterBase const &that) -> EventEmitterBase & = default;
        auto operator=(EventEmitterBase &&that) noexcept -> EventEmitterBase & = default;
    };
} // namespace helium::events::details

export namespace helium::events {
    template<EventEmitterPolicy Policy_>
    class EventEmitter
        : public details::InheritFromMixins<
                  details::EventEmitterBase<Policy_, void>,
                  typename details::SelectMixins<Policy_, details::hasTypeMixins<Policy_>()>::Type>::Type,
          public base::HeliumObject {
    public:
        using super = typename details::InheritFromMixins<
                details::EventEmitterBase<Policy_, void>,
                typename details::SelectMixins<Policy_, details::hasTypeMixins<Policy_>()>::Type>::Type;

    public:
        using super::super;
    };
} // namespace helium::events
