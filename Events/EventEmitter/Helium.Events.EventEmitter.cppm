/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <algorithm>
#include <compare>
#include <concepts>
#include <string>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(_WIN64)
#define BOOST_UUID_RANDOM_PROVIDER_FORCE_WINCRYPT
#endif

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

export module Helium.Events.EventEmitter;

import Helium.Base.HeliumObject;

import Helium.Events.Concepts;
import Helium.Events.Policy;

export namespace helium::events::details {
    template<concepts::IsEventEmitterPolicy Policy_, typename MixinRoot_>
    class EventEmitterBase {
    protected:
        using ThisType = EventEmitterBase<Policy_, MixinRoot_>;

        using MixinRoot = std::conditional_t<std::same_as<MixinRoot_, void>, ThisType, MixinRoot_>;

        using Policy = Policy_;

        using Mixins = typename SelectMixins<Policy, details::hasTypeMixins<Policy>>::Type;

    public:
        EventEmitterBase() = default;
        EventEmitterBase(EventEmitterBase const &that) = default;
        EventEmitterBase(EventEmitterBase &&that) noexcept = default;
        auto operator=(EventEmitterBase const &that) -> EventEmitterBase & = default;
        auto operator=(EventEmitterBase &&that) noexcept -> EventEmitterBase & = default;
    };
} // namespace helium::events::details

export namespace helium::events {
    template<concepts::IsEventEmitterPolicy Policy_>
    class EventEmitter : public BaseHelper<details::EventEmitterBase<Policy_, void>, Policy_>,
                         public base::HeliumObject {
    private:
        using super = BaseHelper<details::EventEmitterBase<Policy_, void>, Policy_>;

    public:
        using super::super;
    };
} // namespace helium::events
