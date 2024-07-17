/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <fstream>
#include <functional>
#include <iostream>
#include <ranges>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>
#include <thread>

#include <replxx.hxx>

#include <pybind11/pybind11.h>

export module Helium.CLI.CLIMainLoop;

import Helium.Commands;
import Helium.Events;
import Helium.Logger;
import Helium.Config;

namespace repl = replxx;

namespace helium::cli
{
auto logger = logger::SharedLogger::getSharedLogger("CLI", "CLIMainLoop");
}

export namespace helium::cli
{

commands::CommandDispatcher dispatcher;

using syntax_highlight_t = std::vector<std::pair<std::string, repl::Replxx::Color>>;
using keyword_highlight_t = std::unordered_map<std::string, repl::Replxx::Color>;

auto utf8stringCodepointLength(char const *s, std::size_t utf8len) -> int
{
    int codepointLen = 0;
    constexpr unsigned char m4 = 128 + 64 + 32 + 16;
    constexpr unsigned char m3 = 128 + 64 + 32;
    constexpr unsigned char m2 = 128 + 64;
    for (int i = 0; i < utf8len; ++i, ++codepointLen)
    {
        char c = s[i];
        if ((c & m4) == m4)
        {
            i += 3;
        }
        else if ((c & m3) == m3)
        {
            i += 2;
        }
        else if ((c & m2) == m2)
        {
            i += 1;
        }
    }
    return codepointLen;
}

auto completionHook(std::string const &context, int &context_len) -> repl::Replxx::completions_t
{
    std::vector<std::string> suggestions{};
    repl::Replxx::completions_t completions;
    if (context_len <= 1)
    {
        suggestions = dispatcher.getSuggestions(context, 0.0f, true);
    }
    else
    {
        suggestions = dispatcher.getSuggestions(context, 50.0f);
    }
    completions = suggestions | std::views::transform([](auto const &suggestion) {
                      return repl::Replxx::Completion{suggestion, repl::Replxx::Color::CYAN};
                  }) |
                  std::ranges::to<repl::Replxx::completions_t>();
    return completions;
}
auto hintHook(std::string const &context, int &context_len, repl::Replxx::Color &color) -> repl::Replxx::hints_t
{
    repl::Replxx::hints_t hints;
    if (context_len <= 1)
    {
        hints = dispatcher.getSuggestions(context, 0.0f, true);
    }
    else
    {
        hints = dispatcher.getSuggestions(context, 60.0f);
    }
    if (hints.size() == 1)
    {
        color = repl::Replxx::Color::GREEN;
    }
    return hints;
}
auto colorHook(std::string const &context, repl::Replxx::colors_t &colors) -> void
{
    syntax_highlight_t regex_color{
        {"[\\-|+]{0,1}[0-9]+",                    repl::Replxx::Color::YELLOW}, // integers
        {"[\\-|+]{0,1}[0-9]*\\.[0-9]+",           repl::Replxx::Color::YELLOW}, // decimals
        {"[\\-|+]{0,1}[0-9]+e[\\-|+]{0,1}[0-9]+", repl::Replxx::Color::YELLOW}, // scientific notation
        {"\".*?\"",                               repl::Replxx::Color::CYAN  }, // double quotes
        {"\'.*?\'",                               repl::Replxx::Color::CYAN  }, // single quotes
    };
    keyword_highlight_t word_color{
        {"`",  repl::Replxx::Color::BRIGHTCYAN   },
        {"'",  repl::Replxx::Color::CYAN         },
        {"\"", repl::Replxx::Color::CYAN         },
        {"-",  repl::Replxx::Color::BRIGHTBLUE   },
        {"+",  repl::Replxx::Color::BRIGHTBLUE   },
        {"=",  repl::Replxx::Color::BRIGHTBLUE   },
        {"/",  repl::Replxx::Color::BRIGHTBLUE   },
        {"*",  repl::Replxx::Color::BRIGHTBLUE   },
        {"^",  repl::Replxx::Color::BRIGHTBLUE   },
        {".",  repl::Replxx::Color::BRIGHTMAGENTA},
        {"(",  repl::Replxx::Color::BRIGHTMAGENTA},
        {")",  repl::Replxx::Color::BRIGHTMAGENTA},
        {"[",  repl::Replxx::Color::BRIGHTMAGENTA},
        {"]",  repl::Replxx::Color::BRIGHTMAGENTA},
        {"{",  repl::Replxx::Color::BRIGHTMAGENTA},
        {"}",  repl::Replxx::Color::BRIGHTMAGENTA},
    };
    // highlight matching regex sequences
    for (auto const &e : regex_color)
    {
        std::size_t pos = 0;
        std::string str = context;
        std::smatch match;

        while (std::regex_search(str, match, std::regex(e.first)))
        {
            std::string c = match[0];
            std::string prefix(match.prefix().str());
            pos += utf8stringCodepointLength(prefix.c_str(), static_cast<int>(prefix.length()));
            int len(utf8stringCodepointLength(c.c_str(), static_cast<int>(c.length())));

            for (int i = 0; i < len; ++i)
            {
                colors.at(pos + i) = e.second;
            }

            pos += len;
            str = match.suffix();
        }
    }
    auto is_keyword = [](char c) -> bool { return std::isalnum(c) or c == '_'; };
    bool in_word = false;
    std::size_t word_start = 0;
    std::size_t word_end = 0;
    std::size_t color_offset = 0;
    auto do_highlight = [&](std::size_t i) {
        in_word = false;
        std::string const intermission = context.substr(word_end, word_start - word_end);
        color_offset += utf8stringCodepointLength(intermission.c_str(), intermission.length());
        std::size_t const wordLen = i - word_start;
        std::string const keyword = context.substr(word_start, wordLen);
        auto const it = word_color.find(keyword);
        auto color = repl::Replxx::Color::DEFAULT;
        if (it != word_color.end())
        {
            color = it->second;
        }
        for (int k = 0; k < wordLen; ++k)
        {
            repl::Replxx::Color &c = colors.at(color_offset + k);
            if (color != repl::Replxx::Color::DEFAULT)
            {
                c = color;
            }
        }
        color_offset += wordLen;
        word_end = i;
    };
    for (int i = 0; i < static_cast<int>(context.length()); ++i)
    {
        if (not in_word)
        {
            if (is_keyword(context[i]))
            {
                in_word = true;
                word_start = i;
            }
        }
        else if (in_word and not is_keyword(context[i]))
        {
            do_highlight(i);
        }
        if (context[i] != '_' and std::ispunct(context[i]))
        {
            word_start = i;
            do_highlight(i + 1);
        }
    }
    if (in_word)
    {
        do_highlight(context.length());
    }
}
auto modifyHook(std::string &line, int &cursor_position, repl::Replxx *)
{
}

auto message(repl::Replxx &replxx, std::string const &s, char32_t) -> repl::Replxx::ACTION_RESULT
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

    bool cli_loop_continue = true;

    // clang-format off
    dispatcher.registerCommand(
        CommandStringLiteral("#helium")
        .then(
            CommandStringLiteral("exit")
            .execute([&cli_loop_continue](CommandContext const &ctx) -> void {
                cli_loop_continue = false;

                auto event_emitter = events::EventEmitter{events::EventBus::getHeliumEventBus()};
                event_emitter.postponeEvent(events::HeliumStopping{});
            }),
            CommandStringLiteral("help"),
            CommandStringLiteral("status"),
            CommandStringLiteral("server")
            .then(
                CommandStringLiteral("start"),
                CommandStringLiteral("stop"),
                CommandStringLiteral("pause"),
                CommandStringLiteral("resume"),
                CommandStringLiteral("terminate"),
                CommandStringLiteral("status")
            ),
            CommandStringLiteral("plugin")
            .then(
                CommandStringLiteral("list"),
                CommandStringLiteral("load")
                .then(
                    CommandArgumentQuotedString("plugin_name")
                ),
                CommandStringLiteral("unload")
                .then(
                    CommandArgumentQuotedString("plugin_name")
                ),
                CommandStringLiteral("status")
                .then(
                    CommandArgumentQuotedString("plugin_name")
                )
            ),
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
            ),
            CommandStringLiteral("debug")
            .then(
                CommandArgumentBoolean("debug_enable")
                .execute([](CommandContext const &ctx, bool enabled) -> void {
                    if(enabled)
                    {
                        logger->debug("Debug mode enabled.");
                    }
                    else
                    {
                        logger->debug("Debug mode disabled.");
                    }
                })
            ),
            CommandStringLiteral("config")
            .then(
                CommandStringLiteral("load"),
                CommandStringLiteral("save")
            )
        )
    );
    // clang-format on

    CommandSource console_source{"console", "helium_main_console"};

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
    rx.bind_key(repl::Replxx::KEY::F1, [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<F1>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::F2, [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<F2>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::F3, [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<F3>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::F4, [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<F4>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::F5, [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<F5>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::F6, [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<F6>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::F7, [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<F7>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::F8, [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<F8>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::F9, [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<F9>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::F10, [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<F10>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::F11, [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<F11>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::F12, [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<F12>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::F1),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<S-F1>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::F2),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<S-F2>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::F3),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<S-F3>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::F4),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<S-F4>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::F5),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<S-F5>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::F6),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<S-F6>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::F7),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<S-F7>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::F8),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<S-F8>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::F9),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<S-F9>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::F10),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<S-F10>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::F11),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<S-F11>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::F12),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<S-F12>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::F1),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<C-F1>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::F2),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<C-F2>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::F3),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<C-F3>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::F4),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<C-F4>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::F5),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<C-F5>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::F6),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<C-F6>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::F7),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<C-F7>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::F8),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<C-F8>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::F9),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<C-F9>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::F10),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<C-F10>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::F11),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<C-F11>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::F12),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<C-F12>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::TAB),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<S-Tab>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::HOME),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<C-Home>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::HOME),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<S-Home>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::END),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<C-End>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::END),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<S-End>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::PAGE_UP),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<C-PgUp>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::control(repl::Replxx::KEY::PAGE_DOWN),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<C-PgDn>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::LEFT),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<S-Left>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::RIGHT),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<S-Right>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::UP),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<S-Up>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::shift(repl::Replxx::KEY::DOWN),
                [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<S-Down>", std::forward<T0>(PH1)); });
    rx.bind_key(repl::Replxx::KEY::meta('\r'), [&rx]<typename T0>(T0 &&PH1) { return message(rx, "<M-Enter>", std::forward<T0>(PH1)); });

    logger->info("REPL initialized");
    logger->flush();

    char const *c_input = nullptr;
    std::string prompt = "";

    while (cli_loop_continue)
    {
        do
        {
            c_input = rx.input(prompt);
        } while (c_input == nullptr and errno == EAGAIN);

        std::string input_command{c_input};

        if (not input_command.empty())
        {
            namespace py = pybind11;
            try
            {
                if(bool execution_result = dispatcher.tryExecuteCommand(console_source, input_command);not execution_result)
                {
                    logger->error("Command failed to execute");
                }
            }
            catch (py::error_already_set const &py_error)
            {
                logger->error("Command {} failed to execute due to exception : {}", input_command, py_error.what());
            }
            catch (std::exception const &exception)
            {
                logger->error("Command {} failed to execute due to exception : {}", input_command, exception.what());
            }
            catch (...)
            {
                logger->error("Command {} failed to execute due to unknown exception", input_command);
            }
            rx.history_add(input_command);
        }
    }

    rx.history_sync(history_file_path);
    logger->info("Exiting Helium.");
}
} // namespace helium::cli