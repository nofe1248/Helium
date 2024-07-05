/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <any>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(_WIN64)
#define BOOST_UUID_RANDOM_PROVIDER_FORCE_WINCRYPT
#endif

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>

#include <nameof.hpp>

#define FWD(x) ::std::forward<decltype(x)>(x)

export module Helium.Events.EventBus;

import Helium.Base.HeliumObject;
import Helium.Events.Concepts;
import Helium.Events.EventStream;

namespace uuids = boost::uuids;

export namespace helium::events
{
using EventIDType = std::size_t;
}
namespace helium::events
{
using EventListenerIDType = uuids::uuid;
}

namespace helium::events::internal
{
template <typename Event>
auto getEventID() noexcept -> EventIDType
{
    static constexpr auto event_id = std::hash<std::string_view>{}(nameof::nameof_full_type<Event>());
    return event_id;
}
} // namespace helium::events::internal

export namespace helium::events
{
class EventBus final : public base::HeliumObject
{
private:
    class EventStreamHelper
    {
        std::any event_stream;

    public:
        template <concepts::IsEvent EventT>
        constexpr auto getStream(this auto &&self) -> EventStream<EventT> &
        {
            return std::any_cast<EventStream<EventT> &>(self.event_stream);
        }
    };
    std::unordered_map<EventIDType, EventStreamHelper> event_streams_map;

public:
    template <concepts::IsEvent EventT>
    auto listenToEvent(this auto &&self, EventListenerIDType listener_id, std::function<void(EventT const &)> &&callback) -> void
    {
    }
};
} // namespace helium::events
