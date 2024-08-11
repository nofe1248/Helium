/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <atomic>
#include <stop_token>
#include <thread>

export module Helium.Events.EventDispatchThread;

import Helium.Base.HeliumObject;
import Helium.Events.EventBus;
import Helium.Events.EventListener;
import Helium.Events.Helium;
import Helium.Logger;
import Helium.Utils;

namespace helium::events
{
auto event_loop_logger = logger::SharedLogger::getSharedLogger("Events", "MainEventLoop");
}

export namespace helium::events
{
class EventDispatchThread final : public base::HeliumObject
{
private:
    std::jthread thread_{};

    auto mainEventLoop(std::stop_token st) -> void
    {
        event_loop_logger->info("Helium main event thread started");
        auto main_bus = EventBus::getInstancePointer();
        auto listener = EventListener{main_bus};
        while (not st.stop_requested())
        {
            main_bus->processEvents();
        }
        main_bus->processEvents();
        event_loop_logger->info("Helium main event thread stopping");
    }

public:
    static auto getInstance() noexcept -> EventDispatchThread &
    {
        static EventDispatchThread instance;
        return instance;
    }

    auto run() -> void
    {
        this->thread_ = std::move(std::jthread(&EventDispatchThread::mainEventLoop, this));
    }

    auto stop() -> void
    {
        this->thread_.request_stop();
        if (this->thread_.joinable())
        {
            this->thread_.join();
        }
    }

    ~EventDispatchThread()
    {
        this->stop();
    }
};
} // namespace helium::events