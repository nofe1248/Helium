/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <cctype>
#include <optional>
#include <expected>
#include <format>
#include <ranges>
#include <string>

#include <nameof.hpp>

#include <plf_hive.h>

#include <mpark/patterns.hpp>

#include <boost/algorithm/string.hpp>

#include <boost/sml2>

export module Helium.Commands.Lexer;

import Helium.Commands.Concepts;

export namespace helium::commands {
    enum class TokenCategory {
        TOKEN_PLAIN_STRING,
        TOKEN_INTEGER,
        TOKEN_FLOATING_POINT,
        TOKEN_BOOLEAN,
        TOKEN_QUOTED_STRING
    };

    template<concepts::IsString StrType_>
    struct Token {
        using StringType = StrType_;

        TokenCategory token_type;
        StringType token_str;

        auto toString(this auto const &self) -> std::string {
            return std::format("Token[.token_type = {}, .token_str = {}]",
                               nameof::nameof_enum(self.token_type), self.token_str);
        }
    };

    template<concepts::IsString StrType_>
    class CommandLexer {
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

        struct LexError {
            std::string msg;
            auto reportError() const noexcept -> void {

            }
        };

    public:
        CommandLexer() :
            original_command_(), raw_command_(), tokens_(), current_iterator_(this->raw_command_.cbegin()) {}

        auto processCommand(StringViewType const cmd) -> std::optional<TokenHive> {
            this->original_command_ = std::move(cmd);
            this->raw_command_ = boost::trim(this->original_command_);

            std::vector<StringType> raw_tokens;

            boost::split(raw_tokens, this->raw_command_, boost::is_any_of(" "), boost::token_compress_on);

            TokenHive hive;

            for(auto const& raw_token : raw_tokens) {
                if(auto exp = this->tryScanToken(raw_token)) {
                    hive.insert(*exp);
                } else {
                    exp.error().reportError();
                    return std::nullopt;
                }
            }
        }

    private:
        auto tryScanToken(StringViewType raw_token) -> std::expected<TokenType, LexError> {
            if(auto exp = this->tryScanBooleanLiteral(raw_token)) {
                return exp;
            }
            if(auto exp = this->tryScanNumber(raw_token)) {
                return exp;
            }
            if(auto exp = this->tryScanQuotedString(raw_token)) {
                return exp;
            }
            if(auto exp = this->tryScanPlainString(raw_token)) {
                return exp;
            }
            std::unreachable();
        }

        auto tryScanNumber(StringViewType raw_token) -> std::expected<TokenType, LexError> {
            struct Zero { char data; };
            struct OneToNine { char data; };
            struct Exponent { char data; };
            struct Add { char data; };
            struct Negative { char data; };
            struct DecimalPoint { char data; };
            struct Anything {};

            bool is_neg = false;
            bool is_floating_point = false;
            StringType result_str;

            sml::sm number_state_machine = [&is_neg, &result_str, &is_floating_point] {
                auto parse_error = [] {};
                auto negative = [&is_neg, &result_str] (auto const& event) {
                    is_neg = true;
                    result_str += event.data;
                };
                auto add_to_result = [&result_str](auto const& event) {
                    result_str += event.data;
                };
                auto parse_done = [&result_str](auto const& event) {
                    result_str += event.data;
                };
                auto decimal_point = [&result_str, &is_floating_point](auto const& event) {
                    result_str += event.data;
                    is_floating_point = true;
                };
                auto exponent = [&result_str](auto const& event) {
                    result_str += event.data;
                };

                using namespace sml::dsl;

                return transition_table{
                    *
                    "init"_s + event<Zero> / add_to_result              = "zero"_s,
                    "init"_s + event<OneToNine> / add_to_result         = "any1"_s,
                    "init"_s + event<Exponent> / parse_error            = "error"_s,
                    "init"_s + event<Add> / parse_error                 = "error"_s,
                    "init"_s + event<Negative> / negative               = "minus"_s,
                    "init"_s + event<DecimalPoint> / parse_error        = "error"_s,
                    "init"_s + event<Anything> / parse_error            = "error"_s,

                    "minus"_s + event<Zero> / add_to_result             = "zero"_s,
                    "minus"_s + event<OneToNine> / add_to_result        = "any1"_s,
                    "minus"_s + event<Exponent> / parse_error           = "error"_s,
                    "minus"_s + event<Add> / parse_error                = "error"_s,
                    "minus"_s + event<Negative> / parse_error           = "error"_s,
                    "minus"_s + event<DecimalPoint> / parse_error       = "error"_s,
                    "minus"_s + event<Anything> / parse_error           = "error"_s,

                    "zero"_s + event<Zero> / parse_done                 = "done"_s,
                    "zero"_s + event<OneToNine> / parse_done            = "done"_s,
                    "zero"_s + event<Exponent> / exponent               = "exponent"_s,
                    "zero"_s + event<Add> / parse_done                  = "done"_s,
                    "zero"_s + event<Negative> / parse_done             = "done"_s,
                    "zero"_s + event<DecimalPoint> / decimal_point      = "decimal1"_s,
                    "zero"_s + event<Anything> / parse_error            = "error"_s,

                    "any1"_s + event<Zero> / add_to_result              = "any1"_s,
                    "any1"_s + event<OneToNine> / add_to_result         = "any1"_s,
                    "any1"_s + event<Exponent> / exponent               = "exponent"_s,
                    "any1"_s + event<Add> / parse_done                  = "done"_s,
                    "any1"_s + event<Negative> / parse_done             = "done"_s,
                    "any1"_s + event<DecimalPoint> / decimal_point      = "decimal1"_s,
                    "any1"_s + event<Anything> / parse_error            = "error"_s,

                    "decimal1"_s + event<Zero> / add_to_result          = "decimal2"_s,
                    "decimal1"_s + event<OneToNine> / add_to_result     = "decimal2"_s,
                    "decimal1"_s + event<Exponent> / parse_error        = "error"_s,
                    "decimal1"_s + event<Add> / parse_error             = "error"_s,
                    "decimal1"_s + event<Negative> / parse_error        = "error"_s,
                    "decimal1"_s + event<DecimalPoint> / parse_error    = "error"_s,
                    "decimal1"_s + event<Anything> / parse_error        = "error"_s,

                    "decimal2"_s + event<Zero> / add_to_result          = "decimal2"_s,
                    "decimal2"_s + event<OneToNine> / add_to_result     = "decimal2"_s,
                    "decimal2"_s + event<Exponent> / exponent           = "exponent"_s,
                    "decimal2"_s + event<Add> / parse_done              = "done"_s,
                    "decimal2"_s + event<Negative> / parse_done         = "done"_s,
                    "decimal2"_s + event<DecimalPoint> / parse_done     = "done"_s,
                    "decimal2"_s + event<Anything> / parse_error        = "error"_s,

                    "exponent"_s + event<Zero> / add_to_result          = "any2"_s,
                    "exponent"_s + event<OneToNine> / add_to_result     = "any2"_s,
                    "exponent"_s + event<Exponent> / parse_error        = "error"_s,
                    "exponent"_s + event<Add> / add_to_result           = "sign"_s,
                    "exponent"_s + event<Negative> / add_to_result      = "sign"_s,
                    "exponent"_s + event<DecimalPoint> / parse_error    = "error"_s,
                    "exponent"_s + event<Anything> / parse_error        = "error"_s,

                    "sign"_s + event<Zero> / add_to_result              = "any2"_s,
                    "sign"_s + event<OneToNine> / add_to_result         = "any2"_s,
                    "sign"_s + event<Exponent> / parse_error            = "error"_s,
                    "sign"_s + event<Add> / parse_error                 = "error"_s,
                    "sign"_s + event<Negative> / parse_error            = "error"_s,
                    "sign"_s + event<DecimalPoint> / parse_error        = "error"_s,
                    "sign"_s + event<Anything> / parse_error            = "error"_s,

                    "any2"_s + event<Zero> / add_to_result              = "any2"_s,
                    "any2"_s + event<OneToNine> / add_to_result         = "any2"_s,
                    "any2"_s + event<Exponent> / parse_done             = "done"_s,
                    "any2"_s + event<Add> / parse_done                  = "done"_s,
                    "any2"_s + event<Negative> / parse_done             = "done"_s,
                    "any2"_s + event<DecimalPoint> / parse_done         = "done"_s,
                    "any2"_s + event<Anything> / parse_error            = "error"_s,

                    "done"_s + event<Zero> / parse_error                = "error"_s,
                    "done"_s + event<OneToNine> / parse_error           = "error"_s,
                    "done"_s + event<Exponent> / parse_error            = "error"_s,
                    "done"_s + event<Add> / parse_error                 = "error"_s,
                    "done"_s + event<Negative> / parse_error            = "minus"_s,
                    "done"_s + event<DecimalPoint> / parse_error        = "error"_s,
                    "done"_s + event<Anything> / parse_error            = "error"_s,
                };
            };

            for(auto const& raw_char : raw_token) {
                using namespace mpark::patterns;
                match(raw_char)(
                    pattern('0') = [&number_state_machine] {
                        number_state_machine.process_event(Zero{'0'});
                    },
                    pattern(anyof(
                        '1', '2', '3', '4', '5', '6', '7', '8', '9'
                    )) = [&number_state_machine, raw_char] {
                        number_state_machine.process_event(OneToNine{raw_char});
                    },
                    pattern(anyof('e', 'E')) = [&number_state_machine] {
                        number_state_machine.process_event(Exponent{'E'});
                    },
                    pattern('+') = [&number_state_machine] {
                        number_state_machine.process_event(Add{'+'});
                    },
                    pattern('-') = [&number_state_machine] {
                        number_state_machine.process_event(Negative{'-'});
                    },
                    pattern('.') = [&number_state_machine] {
                        number_state_machine.process_event(DecimalPoint{'.'});
                    },
                    pattern(_) = [&number_state_machine] {
                        number_state_machine.process_event(Anything{});
                    }
                );
            }
            using namespace sml::dsl;
            if(number_state_machine.is("error"_s)) {
                return std::unexpected{LexError{std::format("Invalid number {}", raw_token)}};
            }
            if(is_floating_point) {
                return TokenType {
                    TokenCategory::TOKEN_FLOATING_POINT,
                    result_str
                };
            } else {
                return TokenType {
                    TokenCategory::TOKEN_INTEGER,
                    result_str
                };
            }
        }

        auto tryScanBooleanLiteral(StringViewType raw_token) -> std::expected<TokenType, LexError> {
            auto lowered_token = raw_token
                | std::ranges::for_each([](const char c) -> char { return std::tolower(c); })
                | std::ranges::to<std::string>;
            if(lowered_token == "true" or lowered_token == "false") {
                return TokenType {
                    TokenCategory::TOKEN_BOOLEAN,
                    lowered_token
                };
            }
            return std::unexpected{LexError{std::format("Invalid boolean literal {}", raw_token)}}
        }

        auto tryScanQuotedString(StringViewType raw_token) -> std::expected<TokenType, LexError> {

        }

        auto tryScanPlainString(StringViewType raw_token) -> std::expected<TokenType, LexError> {
            return TokenType {
                TokenCategory::TOKEN_PLAIN_STRING,
                raw_token
            };
        }

        auto tryScanCodepoint(StringViewType raw_token) -> std::expected<int, LexError> {
            if(raw_token.front() != 'u') {
                return std::unexpected{LexError{std::format("Invalid unicode codepoint {}", raw_token)}};
            }
            if(raw_token.length() != 5) {
                return std::unexpected{LexError{std::format("Invalid unicode codepoint {}", raw_token)}};
            }
            raw_token.erase(raw_token.begin());

            int codepoint = 0;
            for(const auto& [current, factor] : std::views::zip(raw_token, {12u, 8u, 4u, 0u})) {
                if(current >= '0' and current <= '9') {
                    codepoint += static_cast<int>(
                        (static_cast<unsigned int>(current) - 0x30u) << factor
                    );
                } else if(current >= 'A' and current <= 'F') {
                    codepoint += static_cast<int>(
                        (static_cast<unsigned int>(current) - 0x37u) << factor
                    );
                } else if(current >= 'a' and current <= 'f') {
                    codepoint += static_cast<int>(
                        (static_cast<unsigned int>(current) - 0x57u) << factor
                    );
                } else {
                    return std::unexpected{LexError{std::format("Invalid unicode codepoint {}", raw_token)}};
                }
            }
            return codepoint;
        }
    };
}  // namespace helium::commands
