/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <concepts>
#include <format>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>
#include <charconv>

#include <rfl.hpp>
#include <rfl/json.hpp>

export module Helium.Utils.RText;

import Helium.Utils.OverloadSet;

namespace ranges = std::ranges;
namespace views = std::ranges::views;

namespace helium::utils::rtext::internal
{
template <typename R, typename T>
concept RangeOf = ranges::range<R> and std::same_as<T, ranges::range_value_t<R>>;
}

export namespace helium::utils::rtext
{
class RText;

class RColorClassic
{
private:
    friend class RText;
    struct RColorClassicInternal
    {
        std::string_view name;
        std::string_view mc_code;
        std::string_view console_code;
    };

public:
    constexpr static auto black = RColorClassicInternal{.name = "black", .mc_code = "§0", .console_code = "\x1b[30m"};
    constexpr static auto dark_blue = RColorClassicInternal{.name = "dark_blue", .mc_code = "§1", .console_code = "\x1b[34m"};
    constexpr static auto dark_green = RColorClassicInternal{.name = "dark_green", .mc_code = "§2", .console_code = "\x1b[32m"};
    constexpr static auto dark_aqua = RColorClassicInternal{.name = "dark_aqua", .mc_code = "§3", .console_code = "\x1b[36m"};
    constexpr static auto dark_red = RColorClassicInternal{.name = "dark_red", .mc_code = "§4", .console_code = "\x1b[31m"};
    constexpr static auto dark_purple = RColorClassicInternal{.name = "dark_purple", .mc_code = "§5", .console_code = "\x1b[35m"};
    constexpr static auto gold = RColorClassicInternal{.name = "gold", .mc_code = "§6", .console_code = "\x1b[33m"};
    constexpr static auto gray = RColorClassicInternal{.name = "gray", .mc_code = "§7", .console_code = "\x1b[37m\x1b[2m"};
    constexpr static auto dark_gray = RColorClassicInternal{.name = "dark_gray", .mc_code = "§8", .console_code = "\x1b[37m\x1b[2m"};
    constexpr static auto blue = RColorClassicInternal{.name = "blue", .mc_code = "§9", .console_code = "\x1b[94m"};
    constexpr static auto green = RColorClassicInternal{.name = "green", .mc_code = "§10", .console_code = "\x1b[92m"};
    constexpr static auto aqua = RColorClassicInternal{.name = "aqua", .mc_code = "§11", .console_code = "\x1b[96m"};
    constexpr static auto red = RColorClassicInternal{.name = "red", .mc_code = "§12", .console_code = "\x1b[91m"};
    constexpr static auto light_purple = RColorClassicInternal{.name = "light_purple", .mc_code = "§13", .console_code = "\x1b[95m"};
    constexpr static auto yellow = RColorClassicInternal{.name = "yellow", .mc_code = "§14", .console_code = "\x1b[93m"};
    constexpr static auto white = RColorClassicInternal{.name = "white", .mc_code = "§15", .console_code = "\x1b[37m"};
    constexpr static auto reset = RColorClassicInternal{.name = "reset", .mc_code = "§16", .console_code = "\x1b[0m"};
};
class RStyleClassic final
{
private:
    friend class RText;
    struct RStyleClassicInternal
    {
        std::string_view name;
        std::string_view mc_code;
        std::string_view console_code;
    };

public:
    constexpr static auto bold = RStyleClassicInternal{.name = "bold", .mc_code = "§l", .console_code = "\x1b[1m"};
    constexpr static auto italic = RStyleClassicInternal{.name = "italic", .mc_code = "§o", .console_code = ""};
    constexpr static auto underlined = RStyleClassicInternal{.name = "underlined", .mc_code = "§n", .console_code = ""};
    constexpr static auto strikethrough = RStyleClassicInternal{.name = "strikethrough", .mc_code = "§m", .console_code = ""};
    constexpr static auto obfuscated = RStyleClassicInternal{.name = "obfuscated", .mc_code = "§k", .console_code = ""};
};
class RAction final
{
private:
    friend class RText;
    struct RActionInternal
    {
        std::string_view action;
    };

public:
    constexpr static auto suggest_command = RActionInternal{.action = "suggest_command"};
    constexpr static auto run_command = RActionInternal{.action = "run_command"};
    constexpr static auto open_url = RActionInternal{.action = "open_url"};
    constexpr static auto open_file = RActionInternal{.action = "open_file"};
    constexpr static auto change_page = RActionInternal{.action = "change_page"};
    constexpr static auto copy_to_clipboard = RActionInternal{.action = "copy_to_clipboard"};
};
class RColor final
{
private:
    short red_value = 0;
    short green_value = 0;
    short blue_value = 0;

public:
    RColor(short const r, short const g, short const b) : red_value(r), green_value(g), blue_value(b)
    {
        if (red_value < 0 or green_value < 0 or blue_value < 0 or red_value > 255 or green_value > 255 or blue_value > 255)
        {
            throw std::invalid_argument("Invalid RGB value");
        }
    }

    [[nodiscard]] auto getRed() const noexcept -> short
    {
        return red_value;
    }
    [[nodiscard]] auto getGreen() const noexcept -> short
    {
        return green_value;
    }
    [[nodiscard]] auto getBlue() const noexcept -> short
    {
        return blue_value;
    }

    auto setRed(short const value) noexcept -> void
    {
        red_value = value;
    }
    auto setGreen(short const value) noexcept -> void
    {
        green_value = value;
    }
    auto setBlue(short const value) noexcept -> void
    {
        blue_value = value;
    }

    auto to_string() const -> std::string
    {
        char r[3], g[3], b[3];
        std::to_chars(r, r + 2, this->red_value, 16);
        std::to_chars(g, g + 2, this->green_value, 16);
        std::to_chars(b, b + 2, this->blue_value, 16);
        return std::format("{}{}{}", r, g, b);
    }
};
class RText final
{
private:
    rfl::Generic::Object json_object;
public:
    RText() : json_object{} {}
    RText(RText const &) = default;
    RText(RText &&) noexcept = default;
    auto operator=(RText const &) -> RText & = default;
    auto operator=(RText &&) noexcept -> RText & = default;

    [[nodiscard]] auto toJSONString() const -> std::string
    {
        return rfl::json::write(this->json_object);
    }
    [[nodiscard]] auto toPlainText() const -> std::string
    {
    }
    [[nodiscard]] auto toColoredText() const -> std::string
    {
    }
    [[nodiscard]] auto toLegacyText() const -> std::string
    {
    }
    auto setColor(std::variant<RColor, RColorClassic::RColorClassicInternal> const &color) -> RText &
    {
        std::visit(OverloadSet{
            [this](RColor const &color) {
                this->json_object["color"] = color.to_string();
            },
            [this](RColorClassic::RColorClassicInternal const &color) {
                this->json_object["color"] = std::string{color.name};
            }
        }, color);
        return *this;
    }
    auto setStyle(RStyleClassic const &style) -> RText &
    {
        return *this;
    }
    auto setStyle(internal::RangeOf<RStyleClassic> auto const &style) -> RText &
    {
        return *this;
    }
    auto setClickEvent(RAction const &action, std::string_view value) -> RText &
    {
        return *this;
    }
    auto setHoverText(internal::RangeOf<RText> auto const &hover_text) -> RText &
    {
        return *this;
    }

    friend auto to_string(RText const &rtext) -> std::string
    {
        return rtext.toPlainText();
    }
};
} // namespace helium::utils::rtext

export template <>
struct std::formatter<helium::utils::rtext::RText>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext &ctx) -> typename ParseContext::iterator
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(helium::utils::rtext::RText const &rtext, FormatContext &ctx) const -> typename FormatContext::iterator
    {
        std::ostringstream oss;
        oss << rtext.toPlainText();
        return std::ranges::copy(std::move(oss).str(), ctx.out()).out;
    }
};