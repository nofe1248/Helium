/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <print>
#include <string>
#include <iostream>

#include <plf_nanotimer.h>

#define BOOST_UT_DISABLE_MODULE
#include <boost/ut.hpp>

export module Helium.Commands.Test;

export import Helium.Commands.CommandArgument;
export import Helium.Commands.CommandBase;
export import Helium.Commands.CommandDispatcher;
export import Helium.Commands.CommandLiteral;
export import Helium.Commands.Concepts;
export import Helium.Commands.Lexer;

export namespace helium::commands::test {
	using namespace boost::ut;
	
	auto testModule() -> void {
		boost::ut::test("Helium.Command Module Test (Lexer #1)") = []
		{
			CommandLexer<std::string> lex;
			while(true) {
			    using namespace std::string_literals;
			    std::string str;
			    std::print("Enter command for lexer(std::string) test > ");
			    std::getline(std::cin, str);
			    if(str == "exit") {
			        break;
			    }

                plf::nanotimer timer;

                timer.start();
			    lex.parseCommand(str);
			    std::println("Lexer parsing spends {}ns {}μs {}ms", timer.get_elapsed_ns(), timer.get_elapsed_us(), timer.get_elapsed_ms());
			    std::println("{}", lex.tokenString());
			    expect(lex.isParsed());
			}
		};
		boost::ut::test("Helium.Command Module Test (Dispatcher #1)") = []
		{
			CommandDispatcher dispatcher;

			dispatcher.registerCommand(
				CommandStringLiteral{"#helium"}.then(
					CommandStringLiteral{"user"}.then(
						CommandArgumentInteger<std::uint64_t>{}.require(
							[]() { 
								return true; 
							}
						).execute(
							[]() { 
								std::println("awa"); 
							}
						)
					)
				).then(
					CommandStringLiteral{"event"}.then(
						CommandArgumentInteger<std::uint64_t>{}.execute(
							[]() { 
								std::println("owo"); 
							}
						)
					)
				)
			);
		};
	}
}