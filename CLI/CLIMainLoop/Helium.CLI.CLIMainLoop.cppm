/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <replxx.hxx>

export module Helium.CLI.CLIMainLoop;

import Helium.Commands;
import Helium.Logger;

namespace repl = replxx;

namespace helium::cli
{
auto logger = logger::SharedLogger::getSharedLogger("CLI", "CLIMainLoop");
}

export namespace helium::cli
{

commands::CommandDispatcher dispatcher;

using syntax_highlight_t = std::vector<std::pair<std::string, repl::Replxx::Color>>;
using keywords_highlight_t = std::unordered_map<std::string, repl::Replxx::Color>;

auto completionHook(std::string const &context, int &context_len) -> repl::Replxx::completions_t
{
    repl::Replxx::completions_t completions;
    auto suggestions = dispatcher.getSuggestions(context);
    logger->trace("");
    for(auto const &str : suggestions)
    {
        logger->trace("completions : {}", str);
    }
    return completions;
}
auto hintHook(std::string const &context, int &context_len, repl::Replxx::Color &color) -> repl::Replxx::hints_t
{
    repl::Replxx::hints_t hints;
    auto suggestions = dispatcher.getSuggestions(context);
    logger->trace("");
    for(auto const &str : suggestions)
    {
        logger->trace("hints : {}", str);
    }
    return hints;
}
auto colorHook(std::string const &str, repl::Replxx::colors_t &colors) -> void
{
}
auto modifyHook(std::string &line, int &cursor_position, repl::Replxx *)
{
}

auto message(repl::Replxx &replxx, std::string s, char32_t) -> repl::Replxx::ACTION_RESULT
{
    replxx.invoke(repl::Replxx::ACTION::CLEAR_SELF, 0);
    replxx.print("%s\n", s.c_str());
    replxx.invoke(repl::Replxx::ACTION::REPAINT, 0);
    return repl::Replxx::ACTION_RESULT::CONTINUE;
}

auto mainCLILoop()
{
    logger->info("Entering main CLI loop.");

    using namespace commands;

    // clang-format off
    dispatcher.registerCommand(
        CommandStringLiteral("#helium")
        .then(
            CommandStringLiteral("help"),
            CommandStringLiteral("show")
            .then(
                CommandStringLiteral("warranty")
                .execute([](CommandContext const &ctx) -> void {
                    logger->debug("#helium show warranty command executed.");
                }),
                CommandStringLiteral("copyright")
                .execute([](CommandContext const &ctx) -> void {
                    logger->debug("#helium show copyright command executed.");
                })
            )
        )
    );
    // clang-format on

    CommandSource console_source{};

    repl::Replxx rx;
    rx.install_window_change_handler();
    std::string history_file_path = "./helium_command_history.txt";

    {
        std::ifstream history_file(history_file_path);
        rx.history_load(history_file);
    }

    rx.set_max_history_size(512);
    rx.set_max_hint_rows(5);

    rx.set_completion_callback(completionHook);
    rx.set_hint_callback(hintHook);
    rx.set_highlighter_callback(colorHook);

    rx.set_word_break_characters(" \n\t.,-%!;:=*~^'\"/?<>|[](){}");
    rx.set_completion_count_cutoff(128);
    rx.set_hint_delay(200);
    rx.set_double_tab_completion(true);
    rx.set_complete_on_empty(false);
    rx.set_beep_on_ambiguous_completion(false);
    rx.set_no_color(false);
    rx.set_indent_multiline(true);
    rx.enable_bracketed_paste();

    using namespace std::placeholders;
    rx.bind_key_internal(repl::Replxx::KEY::BACKSPACE, "delete_character_left_of_cursor");
    rx.bind_key_internal(repl::Replxx::KEY::DELETE, "delete_character_under_cursor");
    rx.bind_key_internal(repl::Replxx::KEY::LEFT, "move_cursor_left");
    rx.bind_key_internal(repl::Replxx::KEY::RIGHT, "move_cursor_right");
    rx.bind_key_internal(repl::Replxx::KEY::UP, "line_previous");
    rx.bind_key_internal(repl::Replxx::KEY::DOWN, "line_next");
    rx.bind_key_internal(repl::Replxx::KEY::meta(repl::Replxx::KEY::UP), "history_previous");
    rx.bind_key_internal(repl::Replxx::KEY::meta(repl::Replxx::KEY::DOWN), "history_next");
    rx.bind_key_internal(repl::Replxx::KEY::PAGE_UP, "history_first");
    rx.bind_key_internal(repl::Replxx::KEY::PAGE_DOWN, "history_last");
    rx.bind_key_internal(repl::Replxx::KEY::HOME, "move_cursor_to_begining_of_line");
    rx.bind_key_internal(repl::Replxx::KEY::END, "move_cursor_to_end_of_line");
    rx.bind_key_internal(repl::Replxx::KEY::TAB, "complete_line");
    rx.bind_key_internal(repl::Replxx::KEY::control(repl::Replxx::KEY::LEFT), "move_cursor_one_word_left");
    rx.bind_key_internal(repl::Replxx::KEY::control(repl::Replxx::KEY::RIGHT), "move_cursor_one_word_right");
    rx.bind_key_internal(repl::Replxx::KEY::control(repl::Replxx::KEY::UP), "hint_previous");
    rx.bind_key_internal(repl::Replxx::KEY::control(repl::Replxx::KEY::DOWN), "hint_next");
    rx.bind_key_internal(repl::Replxx::KEY::control(repl::Replxx::KEY::ENTER), "commit_line");
    rx.bind_key_internal(repl::Replxx::KEY::control('R'), "history_incremental_search");
    rx.bind_key_internal(repl::Replxx::KEY::control('W'), "kill_to_begining_of_word");
    rx.bind_key_internal(repl::Replxx::KEY::control('U'), "kill_to_begining_of_line");
    rx.bind_key_internal(repl::Replxx::KEY::control('K'), "kill_to_end_of_line");
    rx.bind_key_internal(repl::Replxx::KEY::control('Y'), "yank");
    rx.bind_key_internal(repl::Replxx::KEY::control('L'), "clear_screen");
    rx.bind_key_internal(repl::Replxx::KEY::control('D'), "send_eof");
    rx.bind_key_internal(repl::Replxx::KEY::control('C'), "abort_line");
    rx.bind_key_internal(repl::Replxx::KEY::control('T'), "transpose_characters");
#ifndef _WIN32
    rx.bind_key_internal(repl::Replxx::KEY::control('V'), "verbatim_insert");
    rx.bind_key_internal(repl::Replxx::KEY::control('Z'), "suspend");
#endif
    rx.bind_key_internal(repl::Replxx::KEY::meta(repl::Replxx::KEY::BACKSPACE), "kill_to_whitespace_on_left");
    rx.bind_key_internal(repl::Replxx::KEY::meta('p'), "history_common_prefix_search");
    rx.bind_key_internal(repl::Replxx::KEY::meta('n'), "history_common_prefix_search");
    rx.bind_key_internal(repl::Replxx::KEY::meta('d'), "kill_to_end_of_word");
    rx.bind_key_internal(repl::Replxx::KEY::meta('y'), "yank_cycle");
    rx.bind_key_internal(repl::Replxx::KEY::meta('u'), "uppercase_word");
    rx.bind_key_internal(repl::Replxx::KEY::meta('l'), "lowercase_word");
    rx.bind_key_internal(repl::Replxx::KEY::meta('c'), "capitalize_word");
    rx.bind_key_internal('a', "insert_character");
    rx.bind_key_internal(repl::Replxx::KEY::INSERT, "toggle_overwrite_mode");
    rx.bind_key(repl::Replxx::KEY::F1, std::bind(&message, std::ref(rx), "<F1>", _1));
    rx.bind_key(repl::Replxx::KEY::F2, std::bind(&message, std::ref(rx), "<F2>", _1));
    rx.bind_key(repl::Replxx::KEY::F3, std::bind(&message, std::ref(rx), "<F3>", _1));
    rx.bind_key(repl::Replxx::KEY::F4, std::bind(&message, std::ref(rx), "<F4>", _1));
    rx.bind_key(repl::Replxx::KEY::F5, std::bind(&message, std::ref(rx), "<F5>", _1));
    rx.bind_key(repl::Replxx::KEY::F6, std::bind(&message, std::ref(rx), "<F6>", _1));
    rx.bind_key(repl::Replxx::KEY::F7, std::bind(&message, std::ref(rx), "<F7>", _1));
    rx.bind_key(repl::Replxx::KEY::F8, std::bind(&message, std::ref(rx), "<F8>", _1));
    rx.bind_key(repl::Replxx::KEY::F9, std::bind(&message, std::ref(rx), "<F9>", _1));
    rx.bind_key(repl::Replxx::KEY::F10, std::bind(&message, std::ref(rx), "<F10>", _1));
    rx.bind_key(repl::Replxx::KEY::F11, std::bind(&message, std::ref(rx), "<F11>", _1));
    rx.bind_key(repl::Replxx::KEY::F12, std::bind(&message, std::ref(rx), "<F12>", _1));
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::F1), std::bind(&message, std::ref(rx), "<S-F1>", _1));
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::F2), std::bind(&message, std::ref(rx), "<S-F2>", _1));
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::F3), std::bind(&message, std::ref(rx), "<S-F3>", _1));
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::F4), std::bind(&message, std::ref(rx), "<S-F4>", _1));
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::F5), std::bind(&message, std::ref(rx), "<S-F5>", _1));
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::F6), std::bind(&message, std::ref(rx), "<S-F6>", _1));
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::F7), std::bind(&message, std::ref(rx), "<S-F7>", _1));
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::F8), std::bind(&message, std::ref(rx), "<S-F8>", _1));
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::F9), std::bind(&message, std::ref(rx), "<S-F9>", _1));
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::F10), std::bind(&message, std::ref(rx), "<S-F10>", _1));
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::F11), std::bind(&message, std::ref(rx), "<S-F11>", _1));
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::F12), std::bind(&message, std::ref(rx), "<S-F12>", _1));
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::F1), std::bind(&message, std::ref(rx), "<C-F1>", _1));
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::F2), std::bind(&message, std::ref(rx), "<C-F2>", _1));
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::F3), std::bind(&message, std::ref(rx), "<C-F3>", _1));
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::F4), std::bind(&message, std::ref(rx), "<C-F4>", _1));
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::F5), std::bind(&message, std::ref(rx), "<C-F5>", _1));
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::F6), std::bind(&message, std::ref(rx), "<C-F6>", _1));
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::F7), std::bind(&message, std::ref(rx), "<C-F7>", _1));
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::F8), std::bind(&message, std::ref(rx), "<C-F8>", _1));
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::F9), std::bind(&message, std::ref(rx), "<C-F9>", _1));
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::F10), std::bind(&message, std::ref(rx), "<C-F10>", _1));
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::F11), std::bind(&message, std::ref(rx), "<C-F11>", _1));
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::F12), std::bind(&message, std::ref(rx), "<C-F12>", _1));
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::TAB), std::bind(&message, std::ref(rx), "<S-Tab>", _1));
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::HOME), std::bind(&message, std::ref(rx), "<C-Home>", _1));
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::HOME), std::bind(&message, std::ref(rx), "<S-Home>", _1));
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::END), std::bind(&message, std::ref(rx), "<C-End>", _1));
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::END), std::bind(&message, std::ref(rx), "<S-End>", _1));
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::PAGE_UP), std::bind(&message, std::ref(rx), "<C-PgUp>", _1));
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::PAGE_DOWN), std::bind(&message, std::ref(rx), "<C-PgDn>", _1));
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::LEFT), std::bind(&message, std::ref(rx), "<S-Left>", _1));
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::RIGHT), std::bind(&message, std::ref(rx), "<S-Right>", _1));
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::UP), std::bind(&message, std::ref(rx), "<S-Up>", _1));
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::DOWN), std::bind(&message, std::ref(rx), "<S-Down>", _1));
    rx.bind_key(repl::Replxx::KEY::meta('\r'), std::bind(&message, std::ref(rx), "<M-Enter>", _1));

    logger->info("REPL initialized");

    char const *c_input = nullptr;
    std::string prompt = "Helium$";
    while (true)
    {
        do
        {
            c_input = rx.input(prompt);
        } while (c_input == nullptr and errno == EAGAIN);

        std::string input_command{c_input};

        if (not input_command.empty())
        {
            bool execution_result = dispatcher.tryExecuteCommand(console_source, input_command);
            rx.history_add(input_command);
        }
        if (input_command == "exit")
        {
            break;
        }
    }
}
} // namespace helium::cli