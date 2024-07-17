/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <stop_token>

export module Helium.Events.MainEventLoop;

import Helium.Events.EventBus;
import Helium.Events.EventListener;
import Helium.Logger;

namespace helium::events
{
auto event_loop_logger = logger::SharedLogger::getSharedLogger("Events", "MainEventLoop");
}

export namespace helium::events
{
auto mainEventLoop(std::stop_token st) -> void {
    event_loop_logger->info("Helium main event thread started");
    auto main_bus = EventBus::getHeliumEventBus();
    while (not st.stop_requested())
    {
        main_bus->processEvents();
    }
    event_loop_logger->info("Helium main event thread stopping");
}
}