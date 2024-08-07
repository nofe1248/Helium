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
} // namespace helium::server