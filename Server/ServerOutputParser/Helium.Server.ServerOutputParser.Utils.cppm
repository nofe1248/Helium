/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <string>

#include <re2/re2.h>

export module Helium.Server.ServerOutputParser.Utils;

export namespace helium::server
{
auto verifyPlayerName(std::string_view player_name) -> bool
{
    RE2 player_name_regex{R"([a-zA-Z0-9_]{3,16})"};
    return RE2::FullMatch(player_name, player_name_regex);
}
auto cleanMinecraftColorCode(std::string const &raw_string) -> std::string
{
    RE2 minecraft_color_code_regex{R"(§[a-z0-9])"};
    std::string_view processed{raw_string};

    while (RE2::Consume(&processed, minecraft_color_code_regex))
    {
    }

    return std::string{processed};
}
} // namespace helium::server