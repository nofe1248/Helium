/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <string>

export module Helium.Utils.RText;

export namespace helium::utils::rtext
{
class RColorClassic
{
private:
    struct RColorClassicInternal
    {
        std::string name;
        std::string mc_code;
        std::string console_code;
    };

public:
    constexpr static RColorClassicInternal black = RColorClassicInternal{.name = "black", .mc_code = "§0", .console_code = "\x1b[30m"};
    constexpr static RColorClassicInternal dark_blue = RColorClassicInternal{.name = "dark_blue", .mc_code = "§1", .console_code = "\x1b[34m"};
    constexpr static RColorClassicInternal dark_green = RColorClassicInternal{.name = "dark_green", .mc_code = "§2", .console_code = "\x1b[32m"};
    constexpr static RColorClassicInternal dark_aqua = RColorClassicInternal{.name = "dark_aqua", .mc_code = "§3", .console_code = "\x1b[36m"};
    constexpr static RColorClassicInternal dark_red = RColorClassicInternal{.name = "dark_red", .mc_code = "§4", .console_code = "\x1b[31m"};
    constexpr static RColorClassicInternal dark_purple = RColorClassicInternal{.name = "dark_purple", .mc_code = "§5", .console_code = "\x1b[35m"};
    constexpr static RColorClassicInternal gold = RColorClassicInternal{.name = "gold", .mc_code = "§6", .console_code = "\x1b[33m"};
    constexpr static RColorClassicInternal gray = RColorClassicInternal{.name = "gray", .mc_code = "§7", .console_code = "\x1b[37m\x1b[2m"};
    constexpr static RColorClassicInternal dark_gray =
        RColorClassicInternal{.name = "dark_gray", .mc_code = "§8", .console_code = "\x1b[37m\x1b[2m"};
    constexpr static RColorClassicInternal blue = RColorClassicInternal{.name = "blue", .mc_code = "§9", .console_code = "\x1b[94m"};
    constexpr static RColorClassicInternal green = RColorClassicInternal{.name = "green", .mc_code = "§10", .console_code = "\x1b[92m"};
    constexpr static RColorClassicInternal aqua = RColorClassicInternal{.name = "aqua", .mc_code = "§11", .console_code = "\x1b[96m"};
    constexpr static RColorClassicInternal red = RColorClassicInternal{.name = "red", .mc_code = "§12", .console_code = "\x1b[91m"};
    constexpr static RColorClassicInternal light_purple =
        RColorClassicInternal{.name = "light_purple", .mc_code = "§13", .console_code = "\x1b[95m"};
    constexpr static RColorClassicInternal yellow = RColorClassicInternal{.name = "yellow", .mc_code = "§14", .console_code = "\x1b[93m"};
    constexpr static RColorClassicInternal white = RColorClassicInternal{.name = "white", .mc_code = "§15", .console_code = "\x1b[37m"};
    constexpr static RColorClassicInternal reset = RColorClassicInternal{.name = "reset", .mc_code = "§16", .console_code = "\x1b[0m"};
};
class RStyleClassic final
{
private:
    struct RStyleClassicInternal
    {
        std::string name;
        std::string mc_code;
        std::string console_code;
    };

public:
    constexpr static RStyleClassicInternal bold = RStyleClassicInternal{.name = "bold", .mc_code = "§l", .console_code = "\x1b[1m"};
    constexpr static RStyleClassicInternal italic = RStyleClassicInternal{.name = "italic", .mc_code = "§o", .console_code = ""};
    constexpr static RStyleClassicInternal underlined = RStyleClassicInternal{.name = "underlined", .mc_code = "§n", .console_code = ""};
    constexpr static RStyleClassicInternal strikethrough = RStyleClassicInternal{.name = "strikethrough", .mc_code = "§m", .console_code = ""};
    constexpr static RStyleClassicInternal obfuscated = RStyleClassicInternal{.name = "obfuscated", .mc_code = "§k", .console_code = ""};
};
class RColor final
{
private:
    int red_value = 0;
    int green_value = 0;
    int blue_value = 0;

public:
};
} // namespace helium::utils::rtext