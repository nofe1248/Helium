/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <any>
#include <functional>
#include <memory>
#include <ranges>
#include <shared_mutex>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(_WIN64)
#define BOOST_UUID_RANDOM_PROVIDER_FORCE_WINCRYPT
#endif

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>

#include <nameof.hpp>

#include <proxy/proxy.h>

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
struct Hash
{
    long long p = 31, m = 1e9 + 7;
    long long hash_value;
    constexpr Hash(std::string_view const &s)
    {
        long long hash_so_far = 0;
        long long p_pow = 1;
        long long const n = s.length();
        for (long long i = 0; i < n; ++i)
        {
            hash_so_far = (hash_so_far + (s[i] - 'a' + 1) * p_pow) % m;
            p_pow = (p_pow * p) % m;
        }
        hash_value = hash_so_far;
    }
    constexpr auto operator==(Hash const &other) const noexcept -> bool
    {
        return hash_value == other.hash_value;
    }
};
template <concepts::IsEvent Event>
constexpr auto getEventID() noexcept -> EventIDType
{
    static constexpr auto event_id = Hash(nameof::nameof_full_type<Event>()).hash_value;
    return event_id;
}
} // namespace helium::events::internal

export namespace helium::events
{
class EventBus final : public base::HeliumObject
{
private:
    std::unordered_map<EventIDType, EventStreamProxy> event_streams_map;
    std::unordered_map<std::string, EventStreamProxy> dynamic_id_event_streams_map;

    mutable std::shared_mutex mutex_streams_;
    mutable std::shared_mutex mutex_dynamic_id_streams_;
    mutable std::shared_mutex mutex_process_;

    template <concepts::IsEvent EventT>
    constexpr auto getEventStream(this auto &&self) -> EventStreamProxy
    {
        std::lock_guard guard(self.mutex_streams_);
        static auto event_id = internal::getEventID<EventT>();
        if (not FWD(self).event_streams_map.contains(event_id))
        {
            static EventStream<EventT> event_stream;
            FWD(self).event_streams_map.emplace(std::make_pair<EventIDType, EventStreamProxy>(internal::getEventID<EventT>(), &event_stream));
        }
        return FWD(self).event_streams_map.at(event_id);
    }

    template <concepts::IsDynamicIDEvent EventT>
    constexpr auto getDynamicIDEventStream(this auto &&self, std::string const &event_id) -> EventStreamProxy
    {
        std::lock_guard guard(self.mutex_dynamic_id_streams_);
        if (not FWD(self).dynamic_id_event_streams_map.contains(event_id))
        {
            auto stream_ptr = std::make_shared<EventStream<EventT>>();
            FWD(self).dynamic_id_event_streams_map.emplace(std::make_pair<std::string, EventStreamProxy>(std::string{event_id}, stream_ptr.get()));
        }
        return FWD(self).dynamic_id_event_streams_map.at(event_id);
    }

public:
    static auto getHeliumEventBus() -> std::shared_ptr<EventBus>
    {
        static std::shared_ptr<EventBus> helium_event_bus = std::make_shared<EventBus>();
        return helium_event_bus;
    }

    auto processEvents(this auto &&self) -> void
    {
        std::lock_guard _(FWD(self).mutex_process_);
        std::unordered_map<EventIDType, EventStreamProxy> streams;
        {
            std::lock_guard write_guard(FWD(self).mutex_streams_);
            std::swap(streams, FWD(self).event_streams_map);
        }

        for (EventStreamProxy &event_stream : streams | std::views::values)
        {
            event_stream->processEvents();
        }

        {
            std::lock_guard write_guard(FWD(self).mutex_streams_);
            std::swap(streams, FWD(self).event_streams_map);
        }

        std::unordered_map<std::string, EventStreamProxy> dynamic_id_streams;
        {
            std::lock_guard write_guard(FWD(self).mutex_streams_);
            std::swap(dynamic_id_streams, FWD(self).dynamic_id_event_streams_map);
        }

        for (EventStreamProxy &event_stream : dynamic_id_streams | std::views::values)
        {
            event_stream->processEvents();
        }

        {
            std::lock_guard write_guard(FWD(self).mutex_streams_);
            std::swap(dynamic_id_streams, FWD(self).dynamic_id_event_streams_map);
        }
    }

    template <concepts::IsEvent EventT>
    auto postponeEvent(this auto &&self, EventT event) -> void
    {
        FWD(self).template getEventStream<EventT>()->postponeEvent(event);
    }

    template <concepts::IsDynamicIDEvent EventT>
    auto postponeDynamicIDEvent(this auto &&self, EventT event) -> void
    {
        FWD(self).template getDynamicIDEventStream<EventT>(event.event_id)->postponeEvent(event);
    }

    template <concepts::IsEvent EventT>
    auto listenToEvent(this auto &&self, EventListenerIDType const &listener_id, std::function<void(EventT const &)> &&callback) -> void
    {
        FWD(self).template getEventStream<EventT>()->addListener(listener_id, std::move(callback));
    }

    template <concepts::IsDynamicIDEvent EventT>
    auto listenToDynamicIDEvent(this auto &&self, std::string const &event_id, EventListenerIDType const &listener_id,
                                std::function<void(EventT const &)> &&callback) -> void
    {
        FWD(self).template getDynamicIDEventStream<EventT>(event_id)->addListener(listener_id, std::move(callback));
    }

    template <concepts::IsEvent EventT>
    [[nodiscard]] auto unlistenToEvent(this auto &&self, EventListenerIDType const &listener_id) -> bool
    {
        return FWD(self).template getEventStream<EventT>()->removeListener(listener_id);
    }

    template <concepts::IsDynamicIDEvent EventT>
    [[nodiscard]] auto unlistenToDynamicIDEvent(this auto &&self, std::string const &event_id, EventListenerIDType const &listener_id) -> bool
    {
        return FWD(self).template getDynamicIDEventStream<EventT>(event_id)->removeListener(listener_id);
    }

    auto unlistenAll(this auto &&self, EventListenerIDType const &listener_id) -> void
    {
        for (EventStreamProxy &stream : FWD(self).event_streams_map | std::views::values)
        {
            stream->removeListener(listener_id);
        }

        for (EventStreamProxy &stream : FWD(self).dynamic_id_event_streams_map | std::views::values)
        {
            stream->removeListener(listener_id);
        }
    }

    template <concepts::IsEvent EventT>
    [[nodiscard]] auto isListeningToEvent(this auto &&self, EventListenerIDType const &listener_id) -> bool
    {
        return FWD(self).template getEventStream<EventT>()->hasListener(listener_id);
    }

    template <concepts::IsDynamicIDEvent EventT>
    [[nodiscard]] auto isListeningToDynamicIDEvent(this auto &&self, std::string const &event_id, EventListenerIDType const &listener_id) -> bool
    {
        return FWD(self).template getDynamicIDEventStream<EventT>(event_id)->hasListener(listener_id);
    }
};
} // namespace helium::events
