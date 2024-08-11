/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <optional>
#include <string>

export module Helium.Commands.CommandSource;

import Helium.Base.HeliumObject;

export namespace helium::commands
{
class CommandSource : base::HeliumObject
{
private:
    std::string source_type_ = "unknown_command_source_type";

private:
    std::string major_source_ = "unknown_command_major_source";
    std::optional<std::string> minor_source_ = std::nullopt;

public:
    constexpr CommandSource(std::string source_type, std::string major_source, std::optional<std::string> minor_source = std::nullopt)
        : source_type_(std::move(source_type)), major_source_(std::move(major_source)), minor_source_(std::move(minor_source))
    {
    }

    constexpr CommandSource() = default;
    constexpr CommandSource(CommandSource const &) = default;
    constexpr CommandSource(CommandSource &&) noexcept = default;

    constexpr CommandSource &operator=(CommandSource const &) = default;
    constexpr CommandSource &operator=(CommandSource &&) noexcept = default;

    ~CommandSource() = default;

    [[nodiscard]] auto getSourceType() const -> std::string
    {
        return source_type_;
    }
    auto setSourceType(std::string const &source_type) -> void
    {
        source_type_ = source_type;
    }
    [[nodiscard]] auto getMajorSource() const -> std::string
    {
        return major_source_;
    }
    auto setMajorSource(std::string const &major_source) -> void
    {
        major_source_ = major_source;
    }
    [[nodiscard]] auto getMinorSource() const -> std::optional<std::string>
    {
        return minor_source_;
    }
    auto setMinorSource(std::optional<std::string> const &minor_source) -> void
    {
        minor_source_ = minor_source;
    }
};
} // namespace helium::commands