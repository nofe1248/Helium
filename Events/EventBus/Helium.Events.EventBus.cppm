/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <any>
#include <functional>
#include <memory>
#include <mutex>
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
import Helium.Events.Helium;
import Helium.Logger;

namespace uuids = boost::uuids;

export namespace helium::events
{
using EventIDType = std::size_t;
}
namespace helium::events
{
using EventListenerIDType = uuids::uuid;
auto bus_logger = logger::SharedLogger::getSharedLogger("Events", "EventBus");
} // namespace helium::events

export namespace helium::events
{
class EventBus final : public base::HeliumObject
{
private:
    std::unordered_map<std::string, EventStreamProxy> event_streams_map{};
    std::unordered_map<std::string, DynamicIDEventStreamProxy> dynamic_id_event_streams_map{};
    std::vector<std::function<void()>> stream_deleters_{};

    mutable std::shared_mutex mutex_streams_;
    mutable std::shared_mutex mutex_streams_dynamic_;
    mutable std::shared_mutex mutex_process_;

    template <concepts::IsEvent EventT>
    constexpr auto getEventStream(this auto &&self) -> EventStreamProxy
    {
        std::lock_guard guard(self.mutex_streams_);
        static auto event_type_name = std::string{nameof::nameof_full_type<EventT>()};
        if (not FWD(self).event_streams_map.contains(event_type_name))
        {
            auto *event_stream_ptr = new EventStream<EventT>{};
            FWD(self).stream_deleters_.emplace_back([event_stream_ptr] { delete event_stream_ptr; });
            FWD(self).event_streams_map[event_type_name] = event_stream_ptr;
        }
        return FWD(self).event_streams_map.at(event_type_name);
    }

    constexpr auto getDynamicIDEventStream(this auto &&self) -> DynamicIDEventStreamProxy
    {
        std::lock_guard guard(self.mutex_streams_dynamic_);
        static auto event_type_name = std::string{nameof::nameof_full_type<PythonEvent>()};
        if (not FWD(self).dynamic_id_event_streams_map.contains(event_type_name))
        {
            auto *event_stream_ptr = new DynamicIDEventStream{};
            FWD(self).stream_deleters_.emplace_back([event_stream_ptr] { delete event_stream_ptr; });
            FWD(self).dynamic_id_event_streams_map[event_type_name] = event_stream_ptr;
        }
        return FWD(self).dynamic_id_event_streams_map.at(event_type_name);
    }

public:
    ~EventBus()
    {
        for (auto const &deleter : this->stream_deleters_)
        {
            deleter();
        }
    }

    static auto getInstancePointer() noexcept -> std::shared_ptr<EventBus>
    {
        static auto instance = std::make_shared<EventBus>();
        return instance;
    }

    auto processEvents(this auto &&self) -> void
    {
        std::lock_guard _(FWD(self).mutex_process_);
        std::unordered_map<std::string, EventStreamProxy> streams = FWD(self).event_streams_map;

        for (EventStreamProxy &event_stream : streams | std::views::values)
        {
            event_stream->processEvents();
        }

        std::unordered_map<std::string, DynamicIDEventStreamProxy> dynamic_id_streams = FWD(self).dynamic_id_event_streams_map;

        for (DynamicIDEventStreamProxy &dynamic_id_event_stream : dynamic_id_streams | std::views::values)
        {
            dynamic_id_event_stream->processEvents();
        }
    }

    template <concepts::IsEvent EventT>
    auto postponeEvent(this auto &&self, EventT &&event) -> void
    {
        FWD(self).template getEventStream<EventT>()->postponeEvent(std::move(event));
    }

    auto postponeDynamicIDEvent(this auto &&self, PythonEvent &&event) -> void
    {
        FWD(self).getDynamicIDEventStream()->postponeEvent(std::move(event));
    }

    template <concepts::IsEvent EventT>
    auto listenToEvent(this auto &&self, EventListenerIDType const &listener_id, std::function<void(EventT const &)> &&callback) -> bool
    {
        return FWD(self).template getEventStream<EventT>()->addListener(listener_id, std::move(callback));
    }

    auto listenToDynamicIDEvent(this auto &&self, std::string const &event_id, EventListenerIDType const &listener_id,
                                std::function<void(PythonEvent const &)> &&callback) -> bool
    {
        return FWD(self).getDynamicIDEventStream()->addListener(listener_id, event_id, std::move(callback));
    }

    template <concepts::IsEvent EventT>
    [[nodiscard]] auto unlistenToEvent(this auto &&self, EventListenerIDType const &listener_id) -> bool
    {
        return FWD(self).template getEventStream<EventT>()->removeListener(listener_id);
    }

    [[nodiscard]] auto unlistenToDynamicIDEvent(this auto &&self, std::string const &event_id, EventListenerIDType const &listener_id) -> bool
    {
        return FWD(self).getDynamicIDEventStream()->removeListener(listener_id, event_id);
    }

    auto unlistenAll(this auto &&self, EventListenerIDType const &listener_id) -> void
    {
        for (EventStreamProxy &stream : FWD(self).event_streams_map | std::views::values)
        {
            stream->removeListener(listener_id);
        }

        for (DynamicIDEventStreamProxy &stream : FWD(self).dynamic_id_event_streams_map | std::views::values)
        {
            stream->removeListener(listener_id);
        }
    }

    template <concepts::IsEvent EventT>
    [[nodiscard]] auto isListeningToEvent(this auto &&self, EventListenerIDType const &listener_id) -> bool
    {
        return FWD(self).template getEventStream<EventT>()->hasListener(listener_id);
    }

    [[nodiscard]] auto isListeningToDynamicIDEvent(this auto &&self, std::string const &event_id, EventListenerIDType const &listener_id) -> bool
    {
        return FWD(self).getDynamicIDEventStream()->hasListener(listener_id, event_id);
    }
};
} // namespace helium::events
