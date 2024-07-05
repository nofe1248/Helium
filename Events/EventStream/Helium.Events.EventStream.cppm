/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <functional>
#include <shared_mutex>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/uuid/uuid.hpp>

#define FWD(x) ::std::forward<decltype(x)>(x)

export module Helium.Events.EventStream;

import Helium.Base.HeliumObject;
import Helium.Events.Concepts;

namespace uuids = boost::uuids;

namespace helium::events
{
using EventListenerIDType = uuids::uuid;
}

export namespace helium::events
{
template <concepts::IsEvent EventT>
class EventStream final : public base::HeliumObject
{
public:
    using EventType = EventT;
    using CallbackType = std::function<void(EventType const &event)>;

private:
    std::vector<EventType> event_queue_;
    std::unordered_map<EventListenerIDType, CallbackType> callbacks_;

    mutable std::shared_mutex mutex_event_;

public:
    auto postponeEvent(this auto &&self, EventType event)
    {
        std::lock_guard write_guard(FWD(self).mutex_event_);
        FWD(self).event_queue_.push_back(event);
    }
};
} // namespace helium::events