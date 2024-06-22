/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <cctype>
#include <expected>
#include <format>
#include <optional>
#include <ranges>
#include <string>

#include <nameof.hpp>

#include <plf_hive.h>

#include <mpark/patterns.hpp>

#include <boost/algorithm/string.hpp>

#include <boost/sml2>

export module Helium.Commands.Lexer;

import Helium.Commands.Concepts;
import Helium.Logger;

export namespace helium::commands
{

auto logger = logger::SharedLogger::getSharedLogger("Command", "CommandLexer");

enum class TokenCategory
{
    TOKEN_PLAIN_STRING,
    TOKEN_INTEGER,
    TOKEN_FLOATING_POINT,
    TOKEN_BOOLEAN,
    TOKEN_QUOTED_STRING
};

template <concepts::IsString StrType_ = std::string> struct Token
{
    using StringType = StrType_;

    TokenCategory token_type;
    StringType token_str;

    auto toString(this auto const &self) -> std::string
    {
        return std::format("Token[.token_type = {}, .token_str = {}]", nameof::nameof_enum(self.token_type), self.token_str);
    }
};

template <concepts::IsString StrType_ = std::string> class CommandLexer
{
public:
    using StringType = StrType_;
    using StringViewType = std::basic_string_view<typename StringType::value_type>;
    using TokenType = Token<StringType>;
    using TokenHive = plf::hive<TokenType>;

private:
    StringType original_command_;
    StringType raw_command_;
    TokenHive tokens_;
    typename StringType::const_iterator current_iterator_;

    struct LexError
    {
        std::string msg;
    };

public:
    CommandLexer() : original_command_(), raw_command_(), tokens_(), current_iterator_(this->raw_command_.cbegin())
    {
    }

    auto processCommand(StringType const &cmd) -> std::optional<TokenHive>
    {
        this->original_command_ = std::move(cmd);
        this->raw_command_ = boost::trim_copy(this->original_command_);
        this->current_iterator_ = this->raw_command_.cbegin();

        TokenHive hive;

        while (this->current_iterator_ < this->raw_command_.cend())
        {
            if (auto exp = this->tryScanToken())
            {
                hive.insert(exp.value());
            }
            else
            {
                logger->error("{}", exp.error().msg);
                return std::nullopt;
            }
        }

        return hive;
    }

private:
    auto skipWhitespace() noexcept -> bool
    {
        while (this->current_iterator_ < this->raw_command_.cend() and std::isblank(this->getCurrent().value()))
        {
            ++this->current_iterator_;
        }
        if (this->current_iterator_ < this->raw_command_.cend())
        {
            return true;
        }
        return false;
    }

    auto currentColumn() const noexcept -> std::size_t
    {
        return std::distance(this->raw_command_.cbegin(), this->current_iterator_);
    }

    auto getNext() noexcept -> std::optional<typename StringType::value_type>
    {
        ++this->current_iterator_;
        if (this->current_iterator_ >= this->raw_command_.cend())
        {
            logger->trace("getNext() called, current_iterator out of bound.");
            return std::nullopt;
        }
        logger->trace("getNext() called, current character: {}", *this->current_iterator_);
        return *this->current_iterator_;
    }

    auto getCurrent() const noexcept -> std::optional<typename StringType::value_type>
    {
        if (this->current_iterator_ >= this->raw_command_.cend())
        {
            return std::nullopt;
        }
        return *this->current_iterator_;
    }

    auto tryScanToken() -> std::expected<TokenType, LexError>
    {
        logger->trace("Skipping whitespaces");
        if (this->skipWhitespace())
        {
            char cur = this->getCurrent().value();
            if (std::isdigit(cur))
            {
                logger->trace("Try to scan a number");
                return this->tryScanNumber();
            }
            if (cur == '\"')
            {
                logger->trace("Try to scan a quoted string");
                return this->tryScanQuotedString();
            }
            if (auto exp = this->tryScanBooleanLiteral())
            {
                logger->trace("Try to scan a boolean literal");
                return exp;
            }
            logger->trace("Try to scan a plain string");
            return this->tryScanPlainString();
        }
        return std::unexpected{LexError{"End of string"}};
    }

    auto tryScanNumber() -> std::expected<TokenType, LexError>
    {
        struct Zero
        {
            char data;
        };
        struct OneToNine
        {
            char data;
        };
        struct Exponent
        {
            char data;
        };
        struct Add
        {
            char data;
        };
        struct Negative
        {
            char data;
        };
        struct DecimalPoint
        {
            char data;
        };
        struct Anything
        {
        };

        bool is_neg = false;
        bool is_floating_point = false;
        StringType result_str;

        sml::sm number_state_machine = [&is_neg, &result_str, &is_floating_point] {
            auto parse_error = [] {};
            auto negative = [&is_neg, &result_str](auto const &event) {
                is_neg = true;
                result_str += event.data;
            };
            auto add_to_result = [&result_str](auto const &event) { result_str += event.data; };
            auto parse_done = [&result_str](auto const &event) { result_str += event.data; };
            auto decimal_point = [&result_str, &is_floating_point](auto const &event) {
                result_str += event.data;
                is_floating_point = true;
            };
            auto exponent = [&result_str](auto const &event) { result_str += event.data; };

            using namespace sml::dsl;

            return transition_table{
                *"init"_s + event<Zero> / add_to_result = "zero"_s,
                "init"_s + event<OneToNine> / add_to_result = "any1"_s,
                "init"_s + event<Exponent> / parse_error = "error"_s,
                "init"_s + event<Add> / parse_error = "error"_s,
                "init"_s + event<Negative> / negative = "minus"_s,
                "init"_s + event<DecimalPoint> / parse_error = "error"_s,
                "init"_s + event<Anything> / parse_error = "error"_s,

                "minus"_s + event<Zero> / add_to_result = "zero"_s,
                "minus"_s + event<OneToNine> / add_to_result = "any1"_s,
                "minus"_s + event<Exponent> / parse_error = "error"_s,
                "minus"_s + event<Add> / parse_error = "error"_s,
                "minus"_s + event<Negative> / parse_error = "error"_s,
                "minus"_s + event<DecimalPoint> / parse_error = "error"_s,
                "minus"_s + event<Anything> / parse_error = "error"_s,

                "zero"_s + event<Zero> / parse_done = "done"_s,
                "zero"_s + event<OneToNine> / parse_done = "done"_s,
                "zero"_s + event<Exponent> / exponent = "exponent"_s,
                "zero"_s + event<Add> / parse_done = "done"_s,
                "zero"_s + event<Negative> / parse_done = "done"_s,
                "zero"_s + event<DecimalPoint> / decimal_point = "decimal1"_s,
                "zero"_s + event<Anything> / parse_error = "error"_s,

                "any1"_s + event<Zero> / add_to_result = "any1"_s,
                "any1"_s + event<OneToNine> / add_to_result = "any1"_s,
                "any1"_s + event<Exponent> / exponent = "exponent"_s,
                "any1"_s + event<Add> / parse_done = "done"_s,
                "any1"_s + event<Negative> / parse_done = "done"_s,
                "any1"_s + event<DecimalPoint> / decimal_point = "decimal1"_s,
                "any1"_s + event<Anything> / parse_error = "error"_s,

                "decimal1"_s + event<Zero> / add_to_result = "decimal2"_s,
                "decimal1"_s + event<OneToNine> / add_to_result = "decimal2"_s,
                "decimal1"_s + event<Exponent> / parse_error = "error"_s,
                "decimal1"_s + event<Add> / parse_error = "error"_s,
                "decimal1"_s + event<Negative> / parse_error = "error"_s,
                "decimal1"_s + event<DecimalPoint> / parse_error = "error"_s,
                "decimal1"_s + event<Anything> / parse_error = "error"_s,

                "decimal2"_s + event<Zero> / add_to_result = "decimal2"_s,
                "decimal2"_s + event<OneToNine> / add_to_result = "decimal2"_s,
                "decimal2"_s + event<Exponent> / exponent = "exponent"_s,
                "decimal2"_s + event<Add> / parse_done = "done"_s,
                "decimal2"_s + event<Negative> / parse_done = "done"_s,
                "decimal2"_s + event<DecimalPoint> / parse_done = "done"_s,
                "decimal2"_s + event<Anything> / parse_error = "error"_s,

                "exponent"_s + event<Zero> / add_to_result = "any2"_s,
                "exponent"_s + event<OneToNine> / add_to_result = "any2"_s,
                "exponent"_s + event<Exponent> / parse_error = "error"_s,
                "exponent"_s + event<Add> / add_to_result = "sign"_s,
                "exponent"_s + event<Negative> / add_to_result = "sign"_s,
                "exponent"_s + event<DecimalPoint> / parse_error = "error"_s,
                "exponent"_s + event<Anything> / parse_error = "error"_s,

                "sign"_s + event<Zero> / add_to_result = "any2"_s,
                "sign"_s + event<OneToNine> / add_to_result = "any2"_s,
                "sign"_s + event<Exponent> / parse_error = "error"_s,
                "sign"_s + event<Add> / parse_error = "error"_s,
                "sign"_s + event<Negative> / parse_error = "error"_s,
                "sign"_s + event<DecimalPoint> / parse_error = "error"_s,
                "sign"_s + event<Anything> / parse_error = "error"_s,

                "any2"_s + event<Zero> / add_to_result = "any2"_s,
                "any2"_s + event<OneToNine> / add_to_result = "any2"_s,
                "any2"_s + event<Exponent> / parse_done = "done"_s,
                "any2"_s + event<Add> / parse_done = "done"_s,
                "any2"_s + event<Negative> / parse_done = "done"_s,
                "any2"_s + event<DecimalPoint> / parse_done = "done"_s,
                "any2"_s + event<Anything> / parse_error = "error"_s,

                "done"_s + event<Zero> / parse_error = "error"_s,
                "done"_s + event<OneToNine> / parse_error = "error"_s,
                "done"_s + event<Exponent> / parse_error = "error"_s,
                "done"_s + event<Add> / parse_error = "error"_s,
                "done"_s + event<Negative> / parse_error = "minus"_s,
                "done"_s + event<DecimalPoint> / parse_error = "error"_s,
                "done"_s + event<Anything> / parse_error = "error"_s,
            };
        };

        for (; this->current_iterator_ < this->raw_command_.cend();)
        {
            using namespace mpark::patterns;
            if (auto opt = this->getNext())
            {
                char const cur = opt.value();
                if(std::isblank(cur))
                {
                    break;
                }
                match(cur)(
                    pattern('0') = [&number_state_machine] { number_state_machine.process_event(Zero{'0'}); },
                    pattern(anyof('1', '2', '3', '4', '5', '6', '7', '8', '9')) = [&number_state_machine, this, cur] { number_state_machine.process_event(OneToNine{cur}); },
                    pattern(anyof('e', 'E')) = [&number_state_machine] { number_state_machine.process_event(Exponent{'E'}); },
                    pattern('+') = [&number_state_machine] { number_state_machine.process_event(Add{'+'}); },
                    pattern('-') = [&number_state_machine] { number_state_machine.process_event(Negative{'-'}); },
                    pattern('.') = [&number_state_machine] { number_state_machine.process_event(DecimalPoint{'.'}); },
                    pattern(_) = [&number_state_machine] { number_state_machine.process_event(Anything{}); });
            }
        }

        using namespace sml::dsl;
        if (number_state_machine.is("error"_s))
        {
            return std::unexpected{LexError{std::format("Invalid number at col:{}, {}", this->currentColumn(), this->raw_command_)}};
        }

        if (is_floating_point)
        {
            return TokenType{TokenCategory::TOKEN_FLOATING_POINT, result_str};
        }
        else
        {
            return TokenType{TokenCategory::TOKEN_INTEGER, result_str};
        }
    }

    auto tryScanBooleanLiteral() -> std::expected<TokenType, LexError>
    {
        if (std::distance(this->current_iterator_, this->raw_command_.cend()) >= 4 and this->raw_command_.substr(std::distance(this->raw_command_.cbegin(), this->current_iterator_), 4) == "true")
        {
            this->current_iterator_ += 4;
            return TokenType{TokenCategory::TOKEN_BOOLEAN, "true"};
        }
        if (std::distance(this->current_iterator_, this->raw_command_.cend()) >= 5 and this->raw_command_.substr(std::distance(this->raw_command_.cbegin(), this->current_iterator_), 5) == "false")
        {
            this->current_iterator_ += 5;
            return TokenType{TokenCategory::TOKEN_BOOLEAN, "false"};
        }
        return std::unexpected{LexError{"Invalid boolean literal"}};
    }

    auto tryScanQuotedString() -> std::expected<TokenType, LexError>
    {
        StringType result_str;
        auto add_to_result = [&result_str](auto const &c) { result_str += c; };
        auto next_byte_in_range = [this, &add_to_result](std::vector<std::pair<char, char>> const &ranges) -> bool {
            add_to_result(this->getCurrent().value());
            for (auto const &[first, last] : ranges)
            {
                if (auto opt = this->getNext())
                {
                    if (char const cur = opt.value(); cur >= first and cur <= last) [[likely]]
                    {
                        add_to_result(cur);
                    }
                    else
                    {
                        return false;
                    }
                }
                return false;
            }
            return true;
        };
        if (this->getCurrent().value() != '\"')
        {
            std::unexpected{LexError{std::format("Quoted string must start with an open quote {}", this->raw_command_)}};
        }

        bool no_lex_error_occurred = true;
        bool lex_continue = true;
        std::string error_str;

        while (no_lex_error_occurred and lex_continue)
        {
            using namespace mpark::patterns;
            if (auto opt = this->getNext())
            {
                char const cur = opt.value();
                match(cur)(
                    pattern('\"') = [&lex_continue] { lex_continue = false; },
                    pattern('\\') =
                        [this, &add_to_result, &no_lex_error_occurred, &error_str] {
                            match(this->getNext())(
                                pattern(char{-1}) =
                                    [this, &no_lex_error_occurred, &error_str] {
                                        no_lex_error_occurred = false;
                                        error_str = std::format("Invalid string: missing closing quote : {}", this->raw_command_);
                                    },
                                pattern('\"') = [&add_to_result] { add_to_result('\"'); }, pattern('\\') = [&add_to_result] { add_to_result('\\'); },
                                pattern('/') = [&add_to_result] { add_to_result('/'); }, pattern('b') = [&add_to_result] { add_to_result('\b'); },
                                pattern('f') = [&add_to_result] { add_to_result('\f'); }, pattern('n') = [&add_to_result] { add_to_result('\n'); },
                                pattern('r') = [&add_to_result] { add_to_result('\r'); }, pattern('t') = [&add_to_result] { add_to_result('\t'); },
                                pattern('u') =
                                    [this, &add_to_result, &no_lex_error_occurred, &error_str] {
                                        int codepoint = 0;
                                        StringType first_codepoint = this->raw_command_.substr(this->currentColumn(), 5);
                                        if (auto exp = this->tryScanCodepoint())
                                        {
                                            const int codepoint1 = exp.value();
                                            codepoint = codepoint1;
                                            this->current_iterator_ += 5;

                                            if (codepoint1 >= 0xD800 and codepoint1 <= 0xDBFF)
                                            {
                                                if (this->getNext() == '\\' and this->getNext() == 'u') [[likely]]
                                                {
                                                    this->current_iterator_ -= 2;
                                                    if (auto inner_exp = this->tryScanCodepoint())
                                                    {
                                                        const int codepoint2 = inner_exp.value();

                                                        if (codepoint2 >= 0xDC00 and codepoint2 <= 0xDFFF) [[likely]]
                                                        {
                                                            codepoint = static_cast<int>((static_cast<unsigned int>(codepoint1) << 10u) + static_cast<unsigned int>(codepoint2) - 0x35FDC00u);
                                                        }
                                                        else [[unlikely]]
                                                        {
                                                            no_lex_error_occurred = false;
                                                            error_str = std::format("Invalid string: surrogate "
                                                                                    "U+D800..U+DBFF must be followed "
                                                                                    "by U+DC00..U+DFFF : {}",
                                                                                    this->raw_command_);
                                                        }
                                                    }
                                                    else [[unlikely]]
                                                    {
                                                        no_lex_error_occurred = false;
                                                        error_str = inner_exp.error().msg;
                                                    }
                                                }
                                                else [[unlikely]]
                                                {
                                                    no_lex_error_occurred = false;
                                                    error_str = std::format("Invalid string: surrogate U+D800..U+DBFF "
                                                                            "must be followed by U+DC00..U+DFFF : {}",
                                                                            this->raw_command_);
                                                }
                                            }
                                            else
                                            {
                                                if (codepoint1 >= 0xDC00 and codepoint1 <= 0xDFFF) [[unlikely]]
                                                {
                                                    no_lex_error_occurred = false;
                                                    error_str = std::format("Invalid string: surrogate U+DC00..U+DFFF "
                                                                            "must follow U+D800..U+DBFF : {}",
                                                                            this->raw_command_);
                                                }
                                            }
                                        }
                                        else [[unlikely]]
                                        {
                                            no_lex_error_occurred = false;
                                            error_str = exp.error().msg;
                                        }

                                        if (codepoint >= 0x00 and codepoint <= 0x10FFFF) [[likely]]
                                        {
                                            if (codepoint < 0x80)
                                            {
                                                add_to_result(static_cast<char>(codepoint));
                                            }
                                            else if (codepoint <= 0x7FF)
                                            {
                                                add_to_result(static_cast<char>(0xC0u | (static_cast<unsigned int>(codepoint) >> 6u)));
                                                add_to_result(static_cast<char>(0x80u | (static_cast<unsigned int>(codepoint) >> 0x3Fu)));
                                            }
                                            else if (codepoint <= 0xFFFF)
                                            {
                                                add_to_result(static_cast<char>(0xE0u | (static_cast<unsigned int>(codepoint) >> 12u)));
                                                add_to_result(static_cast<char>(0x80u | ((static_cast<unsigned int>(codepoint) >> 6u) & 0x3Fu)));
                                                add_to_result(static_cast<char>(0x80u | (static_cast<unsigned int>(codepoint) & 0x3Fu)));
                                            }
                                            else
                                            {
                                                add_to_result(static_cast<char>(0xF0u | (static_cast<unsigned int>(codepoint) >> 18u)));
                                                add_to_result(static_cast<char>(0x80u | ((static_cast<unsigned int>(codepoint) >> 12u) & 0x3Fu)));
                                                add_to_result(static_cast<char>(0x80u | ((static_cast<unsigned int>(codepoint) >> 6u) & 0x3Fu)));
                                                add_to_result(static_cast<char>(0x80u | (static_cast<unsigned int>(codepoint) & 0x3Fu)));
                                            }
                                        }
                                        else [[unlikely]]
                                        {
                                            std::unreachable();
                                        }
                                    },
                                pattern(_) =
                                    [this, &no_lex_error_occurred, &error_str] {
                                        no_lex_error_occurred = false;
                                        error_str = std::format("Invalid string: bad escape sequence : {}", this->raw_command_);
                                    });
                        },
                    pattern(arg) =
                        [this, &add_to_result, &no_lex_error_occurred, &next_byte_in_range, &error_str](const char c) {
                            if (c >= 0x00 and c <= 0x1F)
                            {
                                const char *mapping[] = {
                                    "NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL", "BS",  "HT", "LF",  "VT",  "FF", "CR", "SO", "SI",
                                    "DLE", "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB", "CAN", "EM", "SUB", "ESC", "FS", "GS", "RS", "US",
                                };

                                no_lex_error_occurred = false;
                                error_str = std::format("Invalid string: control character U+{:#06x} "
                                                        "({}) must be escaped to \\u{:#06x} : {}",
                                                        static_cast<unsigned int>(c), mapping[c], static_cast<unsigned int>(c), this->raw_command_);
                            }
                            else if (c >= 0x0020 and c <= 0x007F)
                            {
                                add_to_result(c);
                            }
                            else if (c >= 0x00C2 and c <= 0x00DF)
                            {
                                if (not next_byte_in_range({{0x0080, 0x00BF}})) [[unlikely]]
                                {
                                    no_lex_error_occurred = false;
                                    error_str = std::format("Invalid string: ill-formed UTF-8 byte : {}", this->raw_command_);
                                }
                            }
                            else if (c == 0x00E0)
                            {
                                if (not next_byte_in_range({{0x00A0, 0x00BF}, {0x0080, 0x00BF}})) [[unlikely]]
                                {
                                    no_lex_error_occurred = false;
                                    error_str = std::format("Invalid string: ill-formed UTF-8 byte : {}", this->raw_command_);
                                }
                            }
                            else if (c >= 0x00E1 and c <= 0x00EC)
                            {
                                if (not next_byte_in_range({{0x0080, 0x00BF}, {0x0080, 0x00BF}})) [[unlikely]]
                                {
                                    no_lex_error_occurred = false;
                                    error_str = std::format("Invalid string: ill-formed UTF-8 byte : {}", this->raw_command_);
                                }
                            }
                            else if (c >= 0x00EE and c <= 0x00EF)
                            {
                                if (not next_byte_in_range({{0x0080, 0x00BF}, {0x0080, 0x00BF}})) [[unlikely]]
                                {
                                    no_lex_error_occurred = false;
                                    error_str = std::format("Invalid string: ill-formed UTF-8 byte : {}", this->raw_command_);
                                }
                            }
                            else if (c == 0x00ED)
                            {
                                if (not next_byte_in_range({{0x0080, 0x009F}, {0x0080, 0x00BF}})) [[unlikely]]
                                {
                                    no_lex_error_occurred = false;
                                    error_str = std::format("Invalid string: ill-formed UTF-8 byte : {}", this->raw_command_);
                                }
                            }
                            else if (c == 0x00F0)
                            {
                                if (not next_byte_in_range({{0x0090, 0x00BF}, {0x0080, 0x00BF}, {0x0080, 0x00BF}})) [[unlikely]]
                                {
                                    no_lex_error_occurred = false;
                                    error_str = std::format("Invalid string: ill-formed UTF-8 byte : {}", this->raw_command_);
                                }
                            }
                            else if (c >= 0x00F1 and c <= 0x00F3)
                            {
                                if (not next_byte_in_range({{0x0080, 0x00BF}, {0x0080, 0x00BF}, {0x0080, 0x00BF}})) [[unlikely]]
                                {
                                    no_lex_error_occurred = false;
                                    error_str = std::format("Invalid string: ill-formed UTF-8 byte : {}", this->raw_command_);
                                }
                            }
                            else if (c == 0x00F4)
                            {
                                if (not next_byte_in_range({{0x00880, 0x008F}, {0x0080, 0x00BF}, {0x0080, 0x00BF}})) [[unlikely]]
                                {
                                    no_lex_error_occurred = false;
                                    error_str = std::format("Invalid string: ill-formed UTF-8 byte : {}", this->raw_command_);
                                }
                            }
                            else
                            {
                                no_lex_error_occurred = false;
                                error_str = std::format("Invalid string: ill-formed UTF-8 byte : {}", this->raw_command_);
                            }
                        });
            }
            else
            {
                no_lex_error_occurred = false;
                error_str = std::format("Invalid string: missing closing quote : {}", this->raw_command_);
            }
        }
        if (no_lex_error_occurred)
        {
            return Token<std::string>{TokenCategory::TOKEN_QUOTED_STRING, result_str};
        }
        return std::unexpected{LexError{error_str}};
    }

    auto tryScanPlainString() -> std::expected<TokenType, LexError>
    {
        auto cur = this->current_iterator_;
        while (this->current_iterator_ < this->raw_command_.cend() and (not std::isblank(*this->current_iterator_)))
        {
            ++this->current_iterator_;
        }
        return TokenType{TokenCategory::TOKEN_PLAIN_STRING, this->raw_command_.substr(std::distance(this->raw_command_.cbegin(), cur), std::distance(cur, this->current_iterator_))};
    }

    auto tryScanCodepoint() -> std::expected<int, LexError>
    {
        if (this->getCurrent() != 'u')
        {
            return std::unexpected{LexError{std::format("Invalid unicode codepoint {}", this->raw_command_)}};
        }
        if (std::distance(this->current_iterator_, this->raw_command_.cend()) <= 6)
        {
            return std::unexpected{LexError{std::format("Invalid unicode codepoint {}", this->raw_command_)}};
        }
        ++this->current_iterator_;

        int codepoint = 0;
        constexpr unsigned int factors[] = {12u, 8u, 4u, 0u};
        auto raw_token = this->raw_command_.substr(std::distance(this->raw_command_.cbegin(), this->current_iterator_), 4);
        for (auto [current, factor] : std::views::zip(raw_token, factors))
        {
            if (current >= '0' and current <= '9')
            {
                codepoint += static_cast<int>((static_cast<unsigned int>(current) - 0x30u) << factor);
            }
            else if (current >= 'A' and current <= 'F')
            {
                codepoint += static_cast<int>((static_cast<unsigned int>(current) - 0x37u) << factor);
            }
            else if (current >= 'a' and current <= 'f')
            {
                codepoint += static_cast<int>((static_cast<unsigned int>(current) - 0x57u) << factor);
            }
            else
            {
                return std::unexpected{LexError{std::format("Invalid unicode codepoint {}", this->raw_command_)}};
            }
        }
        return codepoint;
    }
};
} // namespace helium::commands
