/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Helium DevTeam
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

module;

#include <ranges>
#include <string>
#include <thread>

#include <cxxopts.hpp>

#include <pybind11/embed.h>

export module Helium.Main;

export import Helium.Base;
export import Helium.CLI;
export import Helium.Commands;
export import Helium.Config;
export import Helium.Events;
export import Helium.Exceptions;
export import Helium.Logger;
export import Helium.Parser;
export import Helium.Plugins;
export import Helium.Server;
export import Helium.Utils;

namespace py = pybind11;

namespace helium::main
{
auto logger = logger::SharedLogger::getSharedLogger("Main", "MainThread");
}

struct TestEvent
{
    std::string test;
};

export namespace helium::main
{
auto heliumMain(int argc, const char *argv[]) -> int
{
    logger->info("Helium version {}, copyright Helium DevTeam 2024, distributed under MIT license.", base::helium_version.to_string());
    cxxopts::Options options{"Helium", "A lightweight extension system for any console applications"};

    events::main_event_bus = std::make_shared<events::EventBus>();
    events::EventEmitter event_emitter{events::main_event_bus};
    std::jthread event_thread{};
    {
        py::gil_scoped_release release;
        event_thread = std::move(std::jthread{events::mainEventLoop});
    }

    event_emitter.postponeEvent(events::HeliumStarting{});

    bool b = config::readConfig();

    if (config::config.debug.debug_mode)
    {
        logger::LoggerImpl::enableAllDebugLog();
        logger->debug("Entered debug mode.");
    }

    options.add_options()("runTest", "Execute tests", cxxopts::value<bool>()->default_value("false"));
    options.allow_unrecognised_options();

    auto result = options.parse(argc, argv);

    if (result["runTest"].as<bool>())
    {
        logger->info("{}", "Running module tests...");
        /*base::test::testModule();
        commands::test::testModule();
        config::test::testModule();
        events::test::testModule();
        logger::test::testModule();
        modules::test::testModule();
        utils::test::testModule();*/
    }

    plugins::PluginManager plugin_manager;
    plugin_manager.SearchAndLoadAllPlugins();

    {
        py::gil_scoped_release release;
        event_emitter.postponeEvent(events::HeliumStarted{});
    }

    cli::mainCLILoop();

    config::saveConfig();

    event_emitter.postponeEvent(events::HeliumStopping{});

    logger->info("Stopping main event thread");
    {
        py::gil_scoped_release release;
        event_thread.request_stop();
        event_thread.join();
    }

    events::main_event_bus.reset();

    logger->info("Main event thread stopped");

    return 0;
}
} // namespace helium::main

export auto main(int argc, const char *argv[]) -> int
{
    py::scoped_interpreter _{};
    return helium::main::heliumMain(argc, argv);
}
