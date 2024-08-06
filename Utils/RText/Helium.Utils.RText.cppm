/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <charconv>
#include <concepts>
#include <format>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>

#include <rfl.hpp>
#include <rfl/json.hpp>

#define FWD(x) ::std::forward<decltype(x)>(x)

export module Helium.Utils.RText;

namespace ranges = std::ranges;
namespace views = std::ranges::views;

namespace helium::utils::rtext::internal
{
template <typename R, typename T>
concept RangeOf = ranges::range<R> and std::convertible_to<std::decay_t<T>, ranges::range_value_t<R>>;
}

export namespace helium::utils::rtext
{
class RColorClassic
{
public:
    struct RColorClassicInternal
    {
        std::string_view name;
        std::string_view mc_code;
        std::string_view console_code;

        friend auto operator==(RColorClassicInternal const &lhs, RColorClassicInternal const &rhs) -> bool
        {
            return lhs.name == rhs.name;
        }
    };

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
public:
    struct RStyleClassicInternal
    {
        std::string_view name;
        std::string_view mc_code;
        std::string_view console_code;

        friend auto operator==(RStyleClassicInternal const &lhs, RStyleClassicInternal const &rhs) -> bool
        {
            return lhs.name == rhs.name;
        }
    };

    constexpr static auto bold = RStyleClassicInternal{.name = "bold", .mc_code = "§l", .console_code = "\x1b[1m"};
    constexpr static auto italic = RStyleClassicInternal{.name = "italic", .mc_code = "§o", .console_code = ""};
    constexpr static auto underlined = RStyleClassicInternal{.name = "underlined", .mc_code = "§n", .console_code = ""};
    constexpr static auto strikethrough = RStyleClassicInternal{.name = "strikethrough", .mc_code = "§m", .console_code = ""};
    constexpr static auto obfuscated = RStyleClassicInternal{.name = "obfuscated", .mc_code = "§k", .console_code = ""};
};
class RAction final
{
public:
    struct RActionInternal
    {
        std::string_view action;

        friend auto operator==(RActionInternal const &lhs, RActionInternal const &rhs) -> bool
        {
            return lhs.action == rhs.action;
        }
    };

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

    [[nodiscard]] auto toString() const -> std::string
    {
        std::string r{"  "}, g{"  "}, b{"  "};
        std::to_chars(r.data(), r.data() + r.size(), this->red_value, 16);
        std::to_chars(g.data(), g.data() + g.size(), this->green_value, 16);
        std::to_chars(b.data(), b.data() + b.size(), this->blue_value, 16);
        if (this->red_value <= 16)
        {
            r.erase(r.size() - 1);
            r.insert(r.begin(), '0');
        }
        if (this->green_value <= 16)
        {
            g.erase(g.size() - 1);
            g.insert(g.begin(), '0');
        }
        if (this->blue_value <= 16)
        {
            b.erase(b.size() - 1);
            b.insert(b.begin(), '0');
        }
        return std::format("#{}{}{}", r, g, b) | views::transform([](char c) { return std::toupper(c); }) | ranges::to<std::string>();
    }
};
class RText final
{
private:
    rfl::Generic::Object json_object_;
    std::optional<RColorClassic::RColorClassicInternal> color_;
    std::vector<RStyleClassic::RStyleClassicInternal> styles_;

public:
    RText() : json_object_{}, color_{std::nullopt}, styles_{}
    {
    }
    explicit RText(std::string const &text) : json_object_{}, color_{std::nullopt}, styles_{}
    {
        this->json_object_["text"] = text;
    }
    RText(RText const &) = default;
    RText(RText &&) noexcept = default;
    auto operator=(RText const &) -> RText & = default;
    auto operator=(RText &&) noexcept -> RText & = default;

    [[nodiscard]] auto toJSONString() const -> std::string
    {
        return rfl::json::write(this->json_object_);
    }
    [[nodiscard]] auto toPlainText() -> std::string
    {
        return this->json_object_["text"].to_string().value_or("");
    }
    [[nodiscard]] auto toColoredText() -> std::string
    {
        std::stringstream ss;
        if (this->color_.has_value())
        {
            ss << this->color_.value().console_code;
        }
        for (auto const &style : this->styles_)
        {
            ss << style.console_code;
        }
        auto head = ss.str();
        std::string tail;
        if (not head.empty())
        {
            tail = RColorClassic::reset.console_code;
        }
        return std::format("{}{}{}", head, this->json_object_["text"].to_string().value_or(""), tail);
    }
    [[nodiscard]] auto toLegacyText() -> std::string
    {
        std::stringstream ss;
        if (this->color_.has_value())
        {
            ss << this->color_.value().mc_code;
        }
        for (auto const &style : this->styles_)
        {
            ss << style.mc_code;
        }
        auto head = ss.str();
        std::string tail;
        if (not head.empty())
        {
            tail = RColorClassic::reset.console_code;
        }
        return std::format("{}{}{}", head, this->json_object_["text"].to_string().value_or(""), tail);
    }
    auto setText(std::string const &text) -> RText &
    {
        this->json_object_["text"] = text;
        return *this;
    }
    auto setText(internal::RangeOf<std::string> auto &&texts) -> RText &
    {
        if (std::size(FWD(texts)) > 1)
        {
            this->setText(*std::begin(FWD(texts)));
            rfl::Generic::Array array;
            for (auto const &extra : FWD(texts) | views::drop(1))
            {
                array.push_back(extra);
            }
            this->json_object_["extra"] = array;
        }
        else if (std::size(FWD(texts)) == 1)
        {
            this->setText(*std::begin(FWD(texts)));
        }
        return *this;
    }
    auto setText(std::initializer_list<std::string> texts) -> RText &
    {
        if (texts.size() > 1)
        {
            this->setText(*texts.begin());
            rfl::Generic::Array array;
            for (auto const &extra : texts | views::drop(1))
            {
                array.emplace_back(extra);
            }
            this->json_object_["extra"] = array;
        }
        else if (texts.size() == 1)
        {
            this->setText(*texts.begin());
        }
        return *this;
    }
    auto setFont(std::string const &font) -> RText &
    {
        this->json_object_["font"] = font;
        return *this;
    }
    auto setColor(RColorClassic::RColorClassicInternal const &color) -> RText &
    {
        this->json_object_["color"] = std::string{color.name};
        this->color_ = color;
        return *this;
    }
    auto setColor(RColor const &color) -> RText &
    {
        this->json_object_["color"] = color.toString();
        return *this;
    }
    auto setStyle(RStyleClassic::RStyleClassicInternal const &style) -> RText &
    {
        this->styles_.push_back(style);
        if (style == RStyleClassic::bold)
        {
            this->json_object_["bold"] = true;
        }
        else if (style == RStyleClassic::italic)
        {
            this->json_object_["italic"] = true;
        }
        else if (style == RStyleClassic::underlined)
        {
            this->json_object_["underlined"] = true;
        }
        else if (style == RStyleClassic::strikethrough)
        {
            this->json_object_["strikethrough"] = true;
        }
        else if (style == RStyleClassic::obfuscated)
        {
            this->json_object_["obfuscated"] = true;
        }
        else
        {
            throw std::invalid_argument("Invalid RText style");
        }
        return *this;
    }
    auto setStyle(internal::RangeOf<RStyleClassic::RStyleClassicInternal> auto &&styles) -> RText &
    {
        for (auto const &style : styles)
        {
            this->setStyle(style);
        }
        return *this;
    }
    auto setStyle(std::initializer_list<RStyleClassic::RStyleClassicInternal> styles) -> RText &
    {
        for (auto const &style : styles)
        {
            this->setStyle(style);
        }
        return *this;
    }
    auto setClickEvent(RAction::RActionInternal const &action, std::string const &value) -> RText &
    {
        rfl::Generic::Object event;
        event["action"] = std::string{action.action};
        event["value"] = value;
        this->json_object_["clickEvent"] = event;
        return *this;
    }
    auto setHoverText(std::string const &text) -> RText &
    {
        rfl::Generic::Object hover_text;
        hover_text["action"] = "show_text";
        hover_text["value"] = text;
        this->json_object_["hoverEvent"] = hover_text;
        return *this;
    }
    auto setHoverText(RText const &text) -> RText &
    {
        rfl::Generic::Object hover_text;
        hover_text["action"] = "show_text";
        hover_text["value"] = text.json_object_;
        this->json_object_["hoverEvent"] = hover_text;
        return *this;
    }

    friend auto to_string(RText const &rtext) -> std::string
    {
        return rtext.toJSONString();
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
        oss << rtext.toJSONString();
        return std::ranges::copy(std::move(oss).str(), ctx.out()).out;
    }
};