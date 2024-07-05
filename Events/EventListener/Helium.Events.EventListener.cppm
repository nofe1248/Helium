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

// non-standard
namespace helium::events
{
class EventBus;
}

export module Helium.Events.EventListener;

import Helium.Base.HeliumObject;
import Helium.Events.Concepts;

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

    EventListener(EventListener const &) = default;
    EventListener &operator=(EventListener const &) = default;
    EventListener(EventListener &&) noexcept = default;
    EventListener &operator=(EventListener &&) noexcept = default;

    ~EventListener()
    {
        if (this->event_bus_ != nullptr)
        {
        }
    }

    template <concepts::IsEvent EventT>
    constexpr auto listenToEvent(this auto &&self, std::function<void(EventT const &)> &&callback) -> void
    {
    }
};
} // namespace helium::events
