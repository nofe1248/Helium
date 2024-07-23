/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <atomic>

export module Helium.Events.MainEventLoop;

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
auto mainEventLoop() -> void
{
    event_loop_logger->info("Helium main event thread started");
    auto main_bus = main_event_bus;
    auto listener = EventListener{main_bus};
    std::atomic_bool should_run = true;
    utils::RunLoopExecutor::getInstance().execute([] {});
    listener.listenToEvent<HeliumStopping>([&should_run](HeliumStopping const &event) { should_run = false; });
    while (should_run)
    {
        main_bus->processEvents();
    }
    main_bus->processEvents();
    event_loop_logger->info("Helium main event thread stopping");
}
} // namespace helium::events