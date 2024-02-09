/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <cctype>
#include <format>
#include <iterator>
#include <memory>
#include <optional>
#include <print>
#include <ranges>
#include <span>
#include <string>

#include <plf_hive.h>

#include <gsl/gsl>

#include <scope_guard.hpp>

#include <nameof.hpp>

#include <mpark/patterns.hpp>

export module Helium.Commands.Lexer;

import Helium.Commands.Concepts;

export namespace helium::commands
{
	enum class TokenCategory
	{
		TOKEN_PLAIN_STRING,
		TOKEN_INTEGER,
		TOKEN_FLOATING_POINT,
		TOKEN_BOOLEAN,
		TOKEN_QUOTED_STRING
	};

	template <concepts::IsString StrType_>
	struct Token
	{
		using StringType = StrType_;

		TokenCategory token_type;
		typename StringType::const_iterator begin_iterator;
		std::basic_string_view<typename StringType::value_type> token_view;

		auto toString(this auto const& self) -> std::string
		{
			return std::format(
				"Token[.token_type = {}, .begin_iterator = {}, .token_view = {}]",
				nameof::nameof_enum(self.token_type),
				*self.begin_iterator,
				self.token_view
			);
		}
	};

	template <concepts::IsString StrType_>
	class CommandLexer
	{
	public:
		using StringType = StrType_;
	    using StringViewType = std::basic_string_view<typename StringType::value_type>;
		using TokenType = Token<StringType>;
	    
	private:
		StringType raw_command_;
		plf::hive<TokenType> tokens_;
		typename StringType::const_iterator current_iterator_;

	public:
		CommandLexer()
			:
		raw_command_(),
		tokens_(),
		current_iterator_(this->raw_command_.cbegin())
		{}

		auto parseCommand(this CommandLexer& self, std::string const& command) -> void
		{
			self.raw_command_ = command;
			self.current_iterator_ = self.raw_command_.cbegin();
			self.tokens_.clear();
			self.parseCommand();
		}

		auto parseCommand(this CommandLexer& self) -> void
		{
			if(not self.raw_command_.empty()) 
			{
				while(auto next_token = self.getNextToken())
				{
					self.tokens_.insert(next_token.value());
				}
			}
		}

	    auto tokenString(this CommandLexer const& self) -> std::string {
		    std::string result;
		    result.append("[\n");
		    result.append(std::format("\tRaw Command : {}\n", self.raw_command_));
		    result.append("\tTokens : \n");
		    result.append("\t[\n");
		    for(auto const& token : self.tokens_) {
		        result.append(std::format("\t\t{}\n", token.toString()));
		    }
		    result.append("\t]\n");
		    result.append("]\n");
		    return result;
		}

		auto isParsed(this CommandLexer const& self) noexcept -> bool
		{
			return self.current_iterator_ == self.raw_command_.cend();
		}

		auto getNextToken(this CommandLexer& self) -> std::optional<TokenType>
		{
			for(; self.current_iterator_ < self.raw_command_.cend(); ++self.current_iterator_)
			{
				if(std::isblank(*self.current_iterator_))
				{
					continue;
				}

				if(std::isdigit(*self.current_iterator_) or *self.current_iterator_ == '-')
				{
					bool is_fp = false;
					bool invalid_fp = false;
					typename StringType::const_iterator begin_iterator = self.current_iterator_;
					++self.current_iterator_;

					while(self.current_iterator_ < self.raw_command_.cend())
					{
						auto _ = sg::make_scope_guard([&self] { ++self.current_iterator_; });
						if(*self.current_iterator_ == '.')
						{
							if(is_fp == true)
							{
								invalid_fp = true;
								self.current_iterator_ = begin_iterator;
								break;
							}
							is_fp = true;
							continue;
						}
						if(std::isdigit(*self.current_iterator_))
						{
							continue;
						}
						break;
					}

					if(not invalid_fp) {
						if(not is_fp)
						{
							return TokenType{
								TokenCategory::TOKEN_INTEGER,
								begin_iterator,
								StringViewType{self.raw_command_}.substr(
									std::distance(self.raw_command_.cbegin(), begin_iterator), 
									std::distance(begin_iterator, self.current_iterator_)
								)
							};
						}
						else
						{
							return TokenType{
								TokenCategory::TOKEN_FLOATING_POINT,
								begin_iterator,
								StringViewType{self.raw_command_}.substr(
									std::distance(self.raw_command_.cbegin(), begin_iterator), 
									std::distance(begin_iterator, self.current_iterator_)
								)
							};
						}
					}
				}

				if(*self.current_iterator_ == '\"' or *self.current_iterator_ == '\'')
				{
				    typename StringType::value_type quote = *self.current_iterator_;
					auto iterator = self.current_iterator_;
				    auto begin_iterator = self.current_iterator_;
                    bool find_closing_quote = false;

				    if(std::distance(iterator, self.raw_command_.cend()) <= 1) {
				        break;
				    }
				    auto end_iterator = iterator + 1;
				    for(; end_iterator < self.raw_command_.cend(); ++end_iterator) {
                        if(*end_iterator == quote or
                            (std::distance(self.raw_command_.cbegin(), end_iterator) >= 2 and
                                (end_iterator - 2 >= self.raw_command_.cbegin() and
                                    *end_iterator == quote and
                                    *(end_iterator - 1) == '\\' and
                                    *(end_iterator - 2) == quote)
                                    )
                                ) {
                            find_closing_quote = true;
                            break;
                        }
				    }
				    if(find_closing_quote) {
				        for(; iterator < end_iterator; ++iterator) {
				            typename StringType::value_type escape_value;
				            bool is_valid_escape_sequence = true;
				            if(std::distance(iterator, end_iterator) >= 1 and *iterator == '\\') {
				                using namespace mpark::patterns;
				                auto is_octal_digit = [](StringType::value_type c) {
				                    bool result = false;
				                    match(c)(
                                        pattern(anyof('0', '1', '2', '3', '4', '5', '6', '7')) = [&result] { result = true; }
				                    );
				                    return result;
				                };
				                ++iterator;
				                match(*iterator)(
                                    pattern('\'') = [&escape_value] {
                                        escape_value = '\'';
                                    },
                                    pattern('\"') = [&escape_value] {
                                        escape_value = '\"';
                                    },
                                    pattern('?') = [&escape_value] {
                                        escape_value = '?';
                                    },
                                    pattern('\\') = [&escape_value] {
                                        escape_value = '\\';
                                    },
                                    pattern('a') = [&escape_value] {
                                        escape_value = '\a';
                                    },
                                    pattern('b') = [&escape_value] {
                                        escape_value = '\b';
                                    },
                                    pattern('f') = [&escape_value] {
                                        escape_value = '\f';
                                    },
                                    pattern('n') = [&escape_value] {
                                        escape_value = '\n';
                                    },
                                    pattern('r') = [&escape_value] {
                                        escape_value = '\r';
                                    },
                                    pattern('t') = [&escape_value] {
                                        escape_value = '\t';
                                    },
                                    pattern('v') = [&escape_value] {
                                        escape_value = '\v';
                                    },
                                    pattern(anyof('0', '1', '2', '3', '4', '5', '6', '7', 'o', 'x')) = [&escape_value, &iterator, &is_valid_escape_sequence] {
                                        match(*iterator)(
                                            pattern(anyof('0', '1', '2', '3', '4', '5', '6', '7')) = [&escape_value, &iterator, &is_valid_escape_sequence] {
                                                std::string octal_value_str{**iterator};
                                            },
                                            pattern('o') = [&escape_value, &iterator, &is_valid_escape_sequence] {

                                            },
                                            pattern('x') = [&escape_value, &iterator, &is_valid_escape_sequence] {

                                            }
                                        );
                                    },
                                    pattern(anyof('u', 'U', 'N')) = [&escape_value, &iterator, &is_valid_escape_sequence] {
                                        match(*iterator)(
                                            pattern('u') = [&escape_value, &iterator, &is_valid_escape_sequence] {

                                            },
                                            pattern('U') = [&escape_value, &iterator, &is_valid_escape_sequence] {

                                            },
                                            pattern('N') = [&escape_value, &iterator, &is_valid_escape_sequence] {

                                            }
                                        );
                                    },
                                    pattern(_) = [&is_valid_escape_sequence] {
                                        is_valid_escape_sequence = false;
                                    }
                                );
				            }
				            if(is_valid_escape_sequence) {

				            }
				        }
				        self.current_iterator_ = end_iterator + 1;
				        return TokenType {
				            TokenCategory::TOKEN_QUOTED_STRING,
                            begin_iterator,
                            StringViewType{self.raw_command_}.substr(
                                std::distance(self.raw_command_.cbegin(), begin_iterator) + 1,
                                std::distance(begin_iterator, self.current_iterator_) - 2
                            )
                        };
				    }
				}

				if(*self.current_iterator_ == 't' or *self.current_iterator_ == 'f')
				{
					if(std::distance(self.current_iterator_, self.raw_command_.cend()) >= 4
					    and self.raw_command_.substr(std::distance(self.raw_command_.cbegin(), self.current_iterator_), 4) == "true")
					{
						auto result = TokenType {
							TokenCategory::TOKEN_BOOLEAN,
							self.current_iterator_,
							StringViewType{self.raw_command_}.substr(
								std::distance(self.raw_command_.cbegin(), self.current_iterator_), 
								4
							)
						};
						self.current_iterator_ += 4;
						return result;
					}
					if(std::distance(self.current_iterator_, self.raw_command_.cend()) >= 5 and
					    self.raw_command_.substr(std::distance(self.raw_command_.cbegin(), self.current_iterator_), 5) == "false")
					{
						auto result = TokenType {
							TokenCategory::TOKEN_BOOLEAN,
							self.current_iterator_,
							StringViewType{self.raw_command_}.substr(
								std::distance(self.raw_command_.cbegin(), self.current_iterator_), 
								5
							)
						};
						self.current_iterator_ += 5;
						return result;
					}
				}

				typename StringType::const_iterator begin_iterator = self.current_iterator_;
				while(self.current_iterator_ < self.raw_command_.cend() and (not std::isblank(*self.current_iterator_)))
				{
					++self.current_iterator_;
				}
				return TokenType {
					TokenCategory::TOKEN_PLAIN_STRING,
					begin_iterator,
					StringViewType{self.raw_command_}.substr(
						std::distance(self.raw_command_.cbegin(), begin_iterator), 
						std::distance(begin_iterator, self.current_iterator_)
					)
				};
			}
			return std::nullopt;
		}
	};
}
