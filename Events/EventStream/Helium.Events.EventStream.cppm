/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <any>
#include <atomic>
#include <functional>
#include <mutex>
#include <ranges>
#include <shared_mutex>
#include <unordered_map>
#include <utility>
#include <vector>

#include <nameof.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_hash.hpp>

#include <proxy/proxy.h>

#include <pybind11/chrono.h>
#include <pybind11/embed.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>

#define FWD(x) ::std::forward<decltype(x)>(x)

export module Helium.Events.EventStream;

import Helium.Base.HeliumObject;
import Helium.Events.Helium;
import Helium.Events.Concepts;
import Helium.Logger;

namespace uuids = boost::uuids;
namespace py = pybind11;

namespace helium::events
{
using EventListenerIDType = uuids::uuid;
auto stream_logger = logger::SharedLogger::getSharedLogger("Events", "EventStream");
}

export namespace helium::events
{
template <concepts::IsEvent EventT>
class EventStream final : public base::HeliumObject
{
public:
    using EventType = EventT;
    using CallbackType = std::function<void(EventType const &)>;

private:
    std::vector<EventType> event_queue_ = {};
    std::unordered_map<EventListenerIDType, CallbackType, boost::hash<uuids::uuid>> callbacks_ = {};
    std::unordered_map<EventListenerIDType, CallbackType, boost::hash<uuids::uuid>> waiting_callbacks_ = {};

    std::atomic_bool is_processing_ = false;

    mutable std::shared_mutex mutex_event_;
    mutable std::shared_mutex mutex_callbacks_;

public:
    EventStream() = default;

    EventStream(EventStream const &) noexcept = default;
    EventStream &operator=(EventStream const &) noexcept = default;
    EventStream(EventStream &&) noexcept = default;
    EventStream &operator=(EventStream &&) noexcept = default;

    auto postponeEvent(this auto &&self, std::any &&event) -> void
    {
        std::lock_guard write_guard(FWD(self).mutex_event_);
        FWD(self).event_queue_.push_back(std::move(std::any_cast<EventType>(std::move(event))));
    }

    auto processEvents(this auto &&self) -> void
    {
        if (FWD(self).event_queue_.empty())
        {
            return;
        }
        std::vector<EventType> events{};
        {
            std::lock_guard write_guard(FWD(self).mutex_event_);
            std::swap(events, FWD(self).event_queue_);
        }
        for (auto const &event : events)
        {
            FWD(self).is_processing_ = true;

            for (auto const &callback : FWD(self).callbacks_ | std::views::values)
            {
                callback(event);
            }

            FWD(self).is_processing_ = false;
            FWD(self).flushWaitingCallbacks();
        }
    }

    [[nodiscard]] auto addListener(this auto &&self, EventListenerIDType const &listenerID, std::any &&callback) -> bool
    {
        std::lock_guard write_guard(FWD(self).mutex_callbacks_);
        if (FWD(self).is_processing_)
        {
            FWD(self).waiting_callbacks_.emplace(std::make_pair(listenerID, std::move(std::any_cast<CallbackType>(std::move(callback)))));
            return true;
        }
        return FWD(self).internalAddListener(listenerID, std::move(std::any_cast<CallbackType>(callback)));
    }

    [[nodiscard]] auto removeListener(this auto &&self, EventListenerIDType const &listenerID) -> bool
    {
        std::lock_guard write_guard(FWD(self).mutex_callbacks_);
        if (FWD(self).is_processing_)
        {
            FWD(self).waiting_callbacks_.emplace(std::make_pair(listenerID, CallbackType{}));
            return true;
        }
        return FWD(self).internalRemoveListener(listenerID);
    }

    [[nodiscard]] auto eventQueueSize(this auto &&self) noexcept -> std::size_t
    {
        return FWD(self).event_queue_.size();
    }

    [[nodiscard]] auto hasEvents(this auto &&self) noexcept -> bool
    {
        return not FWD(self).event_queue_.empty();
    }

    [[nodiscard]] auto hasListener(this auto &&self, EventListenerIDType const &listenerID) noexcept -> bool
    {
        return FWD(self).callbacks_.contains(listenerID) or FWD(self).waiting_callbacks_.contains(listenerID);
    }

private:
    auto flushWaitingCallbacks(this auto &&self) -> void
    {
        std::lock_guard write_guard(FWD(self).mutex_callbacks_);
        if (FWD(self).waiting_callbacks_.empty())
        {
            return;
        }
        for (auto &&pair : FWD(self).waiting_callbacks_)
        {
            if (pair.second)
            {
                (void)FWD(self).internalAddListener(pair.first, std::move(pair.second));
            }
            else
            {
                (void)FWD(self).internalRemoveListener(pair.first);
            }
        }
        FWD(self).waiting_callbacks_.clear();
    }
    [[nodiscard]] auto internalAddListener(this auto &&self, EventListenerIDType const &listenerID, CallbackType &&callback) -> bool
    {
        if (FWD(self).callbacks_.contains(listenerID))
        {
            return false;
        }
        FWD(self).callbacks_[listenerID] = std::move(callback);
        return true;
    }
    [[nodiscard]] auto internalRemoveListener(this auto &&self, EventListenerIDType const &listenerID) -> bool
    {
        if (not FWD(self).callbacks_.contains(listenerID))
        {
            return false;
        }
        FWD(self).callbacks_.erase(listenerID);
        return true;
    }
};

class DynamicIDEventStream final : public base::HeliumObject
{
public:
    using CallbackType = std::function<void(PythonEvent const &)>;

private:
    std::vector<PythonEvent> event_queue_ = {};
    std::unordered_map<EventListenerIDType, std::unordered_map<std::string, CallbackType>, boost::hash<uuids::uuid>> callbacks_ = {};
    std::unordered_map<EventListenerIDType, std::unordered_map<std::string, CallbackType>, boost::hash<uuids::uuid>> waiting_callbacks_ = {};

    std::atomic_bool is_processing_ = false;

    mutable std::shared_mutex mutex_event_;
    mutable std::shared_mutex mutex_callbacks_;

public:
    DynamicIDEventStream() = default;

    DynamicIDEventStream(DynamicIDEventStream const & that) noexcept = delete;
    DynamicIDEventStream &operator=(DynamicIDEventStream const &) noexcept = delete;
    DynamicIDEventStream(DynamicIDEventStream &&) noexcept = delete;
    DynamicIDEventStream &operator=(DynamicIDEventStream &&) noexcept = delete;

    auto postponeEvent(this auto &&self, std::any &&event) -> void
    {
        std::lock_guard write_guard(FWD(self).mutex_event_);
        FWD(self).event_queue_.push_back(std::move(std::any_cast<PythonEvent>(std::move(event))));
    }

    auto processEvents(this auto &&self) -> void
    {
        std::lock_guard write_guard(FWD(self).mutex_event_);
        for (auto const &event : FWD(self).event_queue_)
        {
            FWD(self).is_processing_ = true;

            for (auto &callback_map : FWD(self).callbacks_ | std::views::values)
            {
                if (callback_map.contains(event.event_id))
                {
                    callback_map.at(event.event_id)(event);
                }
            }

            FWD(self).is_processing_ = false;
            FWD(self).flushWaitingCallbacks();
        }
        FWD(self).event_queue_.clear();
    }

    [[nodiscard]] auto addListener(this auto &&self, EventListenerIDType const &listenerID, std::string const &event_id, std::any &&callback) -> bool
    {
        std::lock_guard write_guard(FWD(self).mutex_callbacks_);
        if (FWD(self).is_processing_)
        {
            FWD(self).waiting_callbacks_.emplace(
                std::make_pair(listenerID, std::unordered_map<std::string, CallbackType>{
                                               {event_id, std::move(std::any_cast<CallbackType>(std::move(callback)))}
            }));
            return true;
        }
        return FWD(self).internalAddListener(listenerID, event_id, std::move(std::any_cast<CallbackType>(callback)));
    }

    [[nodiscard]] auto removeListener(this auto &&self, EventListenerIDType const &listenerID, std::string const &event_id) -> bool
    {
        std::lock_guard write_guard(FWD(self).mutex_callbacks_);
        if (FWD(self).is_processing_)
        {
            FWD(self).waiting_callbacks_.emplace(std::make_pair(listenerID, std::unordered_map<std::string, CallbackType>{
                                                                                {event_id, CallbackType{}}
            }));
            return true;
        }
        return FWD(self).internalRemoveListener(listenerID, event_id);
    }

    [[nodiscard]] auto removeListener(this auto &&self, EventListenerIDType const &listenerID) -> bool
    {
        std::lock_guard write_guard(FWD(self).mutex_callbacks_);
        if (FWD(self).is_processing_)
        {
            FWD(self).waiting_callbacks_.emplace(std::make_pair(listenerID, std::unordered_map<std::string, CallbackType>{
                                                                                {std::string{}, CallbackType{}}
            }));
            return true;
        }
        return FWD(self).internalRemoveListener(listenerID);
    }

    [[nodiscard]] auto eventQueueSize(this auto &&self) noexcept -> std::size_t
    {
        return FWD(self).event_queue_.size();
    }

    [[nodiscard]] auto hasEvents(this auto &&self) noexcept -> bool
    {
        return not FWD(self).event_queue_.empty();
    }

    [[nodiscard]] auto hasListener(this auto &&self, EventListenerIDType const &listenerID, std::string const &event_id) noexcept -> bool
    {
        if (not FWD(self).callbacks_.contains(listenerID) or not FWD(self).waiting_callbacks_.contains(listenerID))
        {
            return false;
        }
        return FWD(self).callbacks_[listenerID].contains(event_id) or FWD(self).waiting_callbacks_[listenerID].contains(event_id);
    }

    [[nodiscard]] auto hasListener(this auto &&self, EventListenerIDType const &listenerID) noexcept -> bool
    {
        return FWD(self).callbacks_.contains(listenerID) or FWD(self).waiting_callbacks_.contains(listenerID);
    }

private:
    auto flushWaitingCallbacks(this auto &&self) -> void
    {
        std::lock_guard write_guard(FWD(self).mutex_callbacks_);
        if (FWD(self).waiting_callbacks_.empty())
        {
            return;
        }
        for (auto &&callback_map : FWD(self).waiting_callbacks_)
        {
            for (auto &&pair : callback_map.second)
            {
                if (pair.second)
                {
                    (void)FWD(self).internalAddListener(callback_map.first, std::move(pair.first), std::move(pair.second));
                }
                else
                {
                    if (pair.first.empty())
                    {
                        (void)FWD(self).internalRemoveListener(callback_map.first);
                    }
                    else
                    {
                        (void)FWD(self).internalRemoveListener(callback_map.first, std::move(pair.first));
                    }
                }
            }
        }
        FWD(self).waiting_callbacks_.clear();
    }
    [[nodiscard]] auto internalAddListener(this auto &&self, EventListenerIDType const &listenerID, std::string const &event_id,
                                           CallbackType &&callback) -> bool
    {
        if (FWD(self).callbacks_.contains(listenerID))
        {
            return false;
        }
        FWD(self).callbacks_[listenerID][event_id] = std::move(callback);
        return true;
    }
    [[nodiscard]] auto internalRemoveListener(this auto &&self, EventListenerIDType const &listenerID, std::string const &event_id) -> bool
    {
        if (not FWD(self).callbacks_.contains(listenerID))
        {
            return false;
        }
        if (not FWD(self).callbacks_[listenerID].contains(event_id))
        {
            return false;
        }
        FWD(self).callbacks_[listenerID].erase(event_id);
        return true;
    }
    [[nodiscard]] auto internalRemoveListener(this auto &&self, EventListenerIDType const &listenerID) -> bool
    {
        if (not FWD(self).callbacks_.contains(listenerID))
        {
            return false;
        }
        FWD(self).callbacks_.erase(listenerID);
        return true;
    }
};
} // namespace helium::events

namespace helium::events::proxy
{
PRO_DEF_MEM_DISPATCH(MemberPostponeEvent, postponeEvent);
PRO_DEF_MEM_DISPATCH(MemberProcessEvents, processEvents);
PRO_DEF_MEM_DISPATCH(MemberAddListener, addListener);
PRO_DEF_MEM_DISPATCH(MemberRemoveListener, removeListener);
PRO_DEF_MEM_DISPATCH(MemberEventQueueSize, eventQueueSize);
PRO_DEF_MEM_DISPATCH(MemberHasEvents, hasEvents);
PRO_DEF_MEM_DISPATCH(MemberHasListener, hasListener);
} // namespace helium::events::proxy

export namespace helium::events::proxy
{
// clang-format off
struct EventStreamFacade : pro::facade_builder
    ::support_copy<pro::constraint_level::trivial>
    ::add_convention<MemberAddListener, bool(EventListenerIDType const &, std::any)>
    ::add_convention<MemberHasEvents, bool()>
    ::add_convention<MemberHasListener, bool(EventListenerIDType const &)>
    ::add_convention<MemberPostponeEvent, void(std::any)>
    ::add_convention<MemberProcessEvents, void()>
    ::add_convention<MemberRemoveListener, bool(EventListenerIDType const &)>
    ::add_convention<MemberEventQueueSize, std::size_t()>
    ::build {};

struct DynamicIDEventStreamFacade : pro::facade_builder
    ::support_copy<pro::constraint_level::trivial>
    ::add_convention<MemberAddListener, bool(EventListenerIDType const &, std::string const &, std::any)>
    ::add_convention<MemberHasEvents, bool()>
    ::add_convention<MemberHasListener, bool(EventListenerIDType const &, std::string const &), bool(EventListenerIDType const &)>
    ::add_convention<MemberPostponeEvent, void(std::any)>
    ::add_convention<MemberProcessEvents, void()>
    ::add_convention<MemberRemoveListener, bool(EventListenerIDType const &, std::string const &), bool(EventListenerIDType const &)>
    ::add_convention<MemberEventQueueSize, std::size_t()>
    ::build {};
// clang-format on
} // namespace helium::events::proxy

export namespace helium::events
{
using EventStreamProxy = pro::proxy<proxy::EventStreamFacade>;
using DynamicIDEventStreamProxy = pro::proxy<proxy::DynamicIDEventStreamFacade>;
} // namespace helium::events