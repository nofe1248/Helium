/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <concepts>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>

export module Helium.Commands.CommandContext;

import Helium.Base;
import Helium.Commands.CommandSource;

export namespace helium::commands
{
class CommandArgumentGeneric final : public base::HeliumObject
{
public:
    using VariantType = std::variant<bool, std::string, int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t, float, double>;

private:
    VariantType underlying_object_{};

public:
    CommandArgumentGeneric() : underlying_object_()
    {
    }
    CommandArgumentGeneric(CommandArgumentGeneric const &that) : underlying_object_(that.underlying_object_)
    {
    }
    CommandArgumentGeneric(CommandArgumentGeneric &&that) noexcept : underlying_object_(std::move(that.underlying_object_))
    {
    }
    auto operator=(CommandArgumentGeneric const &that) -> CommandArgumentGeneric &
    {
        this->underlying_object_ = that.underlying_object_;
        return *this;
    }
    auto operator=(CommandArgumentGeneric &&that) noexcept -> CommandArgumentGeneric &
    {
        this->underlying_object_ = std::move(that.underlying_object_);
        return *this;
    }
    explicit CommandArgumentGeneric(VariantType const &that) : underlying_object_(that)
    {
    }
    explicit CommandArgumentGeneric(VariantType &&that) noexcept : underlying_object_(std::move(that))
    {
    }
    template <typename T>
        requires std::convertible_to<T, VariantType>
    explicit CommandArgumentGeneric(T const &value) : underlying_object_(value)
    {
    }
    template <typename T>
        requires std::convertible_to<T, VariantType>
    explicit CommandArgumentGeneric(T &&value) noexcept : underlying_object_(std::forward<T>(value))
    {
    }

    ~CommandArgumentGeneric() = default;

    [[nodiscard]] auto getUnderlyingObject() const noexcept -> VariantType const &
    {
        return this->underlying_object_;
    }

    template <typename T>
        requires std::convertible_to<T, VariantType>
    [[nodiscard]] auto cast() const noexcept -> std::optional<T>
    {
        try
        {
            return std::get<T>(this->underlying_object_);
        }
        catch (std::bad_variant_access const &)
        {
            return std::nullopt;
        }
    }

    [[nodiscard]] auto getBool() const noexcept -> std::optional<bool>
    {
        return this->cast<bool>();
    }

    [[nodiscard]] auto getString() const noexcept -> std::optional<std::string>
    {
        return this->cast<std::string>();
    }

    [[nodiscard]] auto getInt8() const noexcept -> std::optional<int8_t>
    {
        return this->cast<int8_t>();
    }

    [[nodiscard]] auto getInt16() const noexcept -> std::optional<int16_t>
    {
        return this->cast<int16_t>();
    }

    [[nodiscard]] auto getInt32() const noexcept -> std::optional<int32_t>
    {
        return this->cast<int32_t>();
    }

    [[nodiscard]] auto getInt64() const noexcept -> std::optional<int64_t>
    {
        return this->cast<int64_t>();
    }

    [[nodiscard]] auto getUInt8() const noexcept -> std::optional<uint8_t>
    {
        return this->cast<uint8_t>();
    }

    [[nodiscard]] auto getUInt16() const noexcept -> std::optional<uint16_t>
    {
        return this->cast<uint16_t>();
    }

    [[nodiscard]] auto getUInt32() const noexcept -> std::optional<uint32_t>
    {
        return this->cast<uint32_t>();
    }

    [[nodiscard]] auto getUInt64() const noexcept -> std::optional<uint64_t>
    {
        return this->cast<uint64_t>();
    }

    [[nodiscard]] auto getFloat() const noexcept -> std::optional<float>
    {
        return this->cast<float>();
    }

    [[nodiscard]] auto getDouble() const noexcept -> std::optional<double>
    {
        return this->cast<double>();
    }
};
class CommandContext final : public base::HeliumObject
{
private:
    CommandSource source_;
    std::unordered_map<std::string, CommandArgumentGeneric> arguments_;

public:
    constexpr CommandContext(CommandSource source) : source_(std::move(source))
    {
    }

    [[nodiscard]] constexpr auto getSource() const noexcept -> CommandSource const &
    {
        return this->source_;
    }

    constexpr auto setSource(CommandSource source) noexcept -> void
    {
        this->source_ = std::move(source);
    }

    constexpr auto addArgument(std::string const &argument_name, CommandArgumentGeneric const &argument) -> void
    {
        this->arguments_[argument_name] = argument;
    }

    [[nodiscard]] constexpr auto getArgument(std::string const &argument_name) const -> std::optional<CommandArgumentGeneric>
    {
        if (this->arguments_.contains(argument_name))
        {
            return this->arguments_.at(argument_name);
        }
        return std::nullopt;
    }

    constexpr auto clearArguments() -> void
    {
        this->arguments_.clear();
    }
};
} // namespace helium::commands