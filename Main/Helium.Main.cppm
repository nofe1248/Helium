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

#include <iostream>
#include <ranges>
#include <string>

#include <cxxopts.hpp>

#include <plf_nanotimer.h>

export module Helium.Main;

export import Helium.Base;
export import Helium.Commands;
export import Helium.Events;
export import Helium.Exceptions;
export import Helium.Logger;
export import Helium.Modules;
export import Helium.Utils;

namespace helium::main
{
auto logger = logger::SharedLogger::getSharedLogger("Main", "MainThread");
}

export namespace helium::main
{
auto heliumMain(int argc, const char *argv[]) -> int
{
    logger->info("Helium version {}", base::helium_version.to_string());
    cxxopts::Options options{"Helium", "A lightweight extension system for any console applications"};
    options.add_options()("runTest", "Execute tests", cxxopts::value<bool>()->default_value("false"));
    options.allow_unrecognised_options();

    auto result = options.parse(argc, argv);
    commands::CommandLexer lex;
    for (;;)
    {
        std::string input;
        std::getline(std::cin, input);
        plf::nanotimer timer;
        timer.start();
        auto opt = lex.processCommand(input);
        auto ns = timer.get_elapsed_ns();
        auto us = timer.get_elapsed_us();
        auto ms = timer.get_elapsed_ms();
        if (opt)
        {
            std::ranges::for_each(opt.value(), [](commands::Token const &tok) { logger->trace("{}", tok.toString()); });
            logger->trace("Command lexing performance: {}ms {}μs {}ns", ms, us, ns);
        }
    }

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
    return 0;
}
} // namespace helium::main

export auto main(int argc, const char *argv[]) -> int
{
    return helium::main::heliumMain(argc, argv);
}
