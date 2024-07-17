/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <memory>
#include <utility>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(_WIN64)
#define BOOST_UUID_RANDOM_PROVIDER_FORCE_WINCRYPT
#endif

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#define FWD(x) ::std::forward<decltype(x)>(x)

export module Helium.Events.EventEmitter;

import Helium.Base.HeliumObject;
import Helium.Events.EventBus;
import Helium.Events.Helium;
import Helium.Events.Concepts;

namespace uuids = boost::uuids;

export namespace helium::events
{
using EventEmitterIDType = uuids::uuid;
class EventEmitter final : public base::HeliumObject
{
private:
    EventEmitterIDType id_ = uuids::random_generator()();
    std::shared_ptr<EventBus> event_bus_ = nullptr;

public:
    explicit EventEmitter() = default;
    explicit EventEmitter(std::shared_ptr<EventBus> event_bus) : event_bus_(event_bus)
    {
    }

    EventEmitter(EventEmitter const &) = default;
    EventEmitter &operator=(EventEmitter const &) = default;
    EventEmitter(EventEmitter &&) noexcept = default;
    EventEmitter &operator=(EventEmitter &&) noexcept = default;

    template <concepts::IsEvent EventT>
    auto postponeEvent(this auto &&self, EventT &&event) -> void
    {
        FWD(self).event_bus_->template postponeEvent<EventT>(std::move(event));
    }

    auto postponeDynamicIDEvent(this auto &&self, PythonEvent &&event) -> void
    {
        FWD(self).event_bus_->postponeDynamicIDEvent(std::move(event));
    }

    constexpr auto getID(this auto &&self) -> EventEmitterIDType
    {
        return FWD(self).id_;
    }

    constexpr auto getEventBus(this auto &&self) -> std::shared_ptr<EventBus>
    {
        return FWD(self).event_bus_;
    }
};
} // namespace helium::events
