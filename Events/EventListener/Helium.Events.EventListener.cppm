/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <exception>
#include <functional>
#include <memory>
#include <utility>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(_WIN64)
#define BOOST_UUID_RANDOM_PROVIDER_FORCE_WINCRYPT
#endif

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>

#define FWD(x) ::std::forward<decltype(x)>(x)

export module Helium.Events.EventListener;

import Helium.Base.HeliumObject;
import Helium.Events.Concepts;
import Helium.Events.EventBus;

namespace uuids = boost::uuids;

export namespace helium::events
{
using EventListenerIDType = uuids::uuid;
class EventListener final : public base::HeliumObject
{
private:
    EventListenerIDType id_ = uuids::random_generator()();
    std::shared_ptr<EventBus> event_bus_ = nullptr;

public:
    explicit EventListener() = default;
    explicit EventListener(std::shared_ptr<EventBus> event_bus) : event_bus_(event_bus)
    {
    }

    EventListener(EventListener const &) = delete;
    EventListener &operator=(EventListener const &) = delete;
    EventListener(EventListener &&that) noexcept
    {
        assert(this != &that);
        if (this->event_bus_ != nullptr)
        {
            this->unlistenAll();
        }
        this->id_ = that.id_;
        this->event_bus_ = std::move(that.event_bus_);
    }
    EventListener &operator=(EventListener &&that) noexcept
    {
        assert(this != &that);
        if (this->event_bus_ != nullptr)
        {
            this->unlistenAll();
        }
        this->id_ = that.id_;
        this->event_bus_ = std::move(that.event_bus_);
        return *this;
    }

    ~EventListener()
    {
        if (this->event_bus_ != nullptr)
        {
            event_bus_->unlistenAll(this->id_);
        }
    }

    template <concepts::IsEvent EventT>
    constexpr auto listenToEvent(this auto &&self, std::function<void(EventT const &)> &&callback) -> void
    {
        assert(FWD(self).event_bus_ != nullptr);
        FWD(self).event_bus_->listenToEvent(FWD(self).id_, std::move(callback));
    }

    template <concepts::IsEvent EventT>
    constexpr auto unlistenToEvent(this auto &&self) -> bool
    {
        assert(FWD(self).event_bus_ != nullptr);
        return FWD(self).event_bus_->unlistenToEvent(FWD(self).id_);
    }

    constexpr auto unlistenAll(this auto &&self) -> void
    {
        assert(FWD(self).event_bus_ != nullptr);
        FWD(self).event_bus_->unlistenAll(FWD(self).id_);
    }

    template <concepts::IsEvent EventT>
    constexpr auto isListeningToEvent(this auto &&self) -> bool
    {
        assert(FWD(self).event_bus_ != nullptr);
        return FWD(self).event_bus_->isListeningToEvent(FWD(self).id_);
    }

    constexpr auto getID(this auto &&self) -> EventListenerIDType
    {
        return FWD(self).id_;
    }

    constexpr auto getEventBus(this auto &&self) -> std::shared_ptr<EventBus>
    {
        return FWD(self).event_bus_;
    }
};
} // namespace helium::events
