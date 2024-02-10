/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <iostream>
#include <print>
#include <string>

#include <plf_nanotimer.h>

#define BOOST_UT_DISABLE_MODULE
#include <boost/ut.hpp>

export module Helium.Commands.Test;

import Helium.Commands.CommandArgument;
import Helium.Commands.CommandBase;
import Helium.Commands.CommandContext;
import Helium.Commands.CommandDispatcher;
import Helium.Commands.CommandLiteral;
import Helium.Commands.Concepts;
import Helium.Commands.Lexer;

export namespace helium::commands::test {
    using namespace boost::ut;

    auto testModule() -> void {
        boost::ut::test("Helium.Command Module Test (Lexer #1)") = [] {
            CommandLexer<std::string> lex;
            while (true) {
                using namespace std::string_literals;
                std::string str;
                std::print("Enter command for lexer(std::string) test > ");
                std::getline(std::cin, str);
                if (str == "exit") {
                    break;
                }

                plf::nanotimer timer;

                timer.start();
                lex.parseCommand(str);
                std::println("Lexer parsing spends {}ns {}μs {}ms", timer.get_elapsed_ns(), timer.get_elapsed_us(),
                             timer.get_elapsed_ms());
                std::println("{}", lex.tokenString());
                expect(lex.isParsed());
            }
        };
        boost::ut::test("Helium.Command Module Test (Dispatcher #1)") = [] {
            CommandDispatcher dispatcher;

            dispatcher.registerCommand(
                    CommandStringLiteral{"#helium"}
                            .then(CommandStringLiteral{"user"}.then(
                                    CommandArgumentInteger<std::uint64_t>{"awa"}
                                            .require([]() { return true; })
                                            .execute([](CommandContext const &) { std::println("awa"); })))
                            .then(CommandStringLiteral{"event"}.then(
                                    CommandArgumentInteger<std::uint64_t>{"owo"}.execute(
                                            [](CommandContext const &) { std::println("owo"); }))));
        };
    }
}