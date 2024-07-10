/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <optional>
#include <string>
#include <vector>
#include <filesystem>

#include <semver.hpp>

export module Helium.Plugins.PluginMetadata;

export namespace helium::plugins
{
struct PluginMetadata
{
    struct PluginDependency
    {
        std::string id;
        semver::range::detail::range version_range;
    };
    std::filesystem::path plugin_path;
    std::string id;
    std::string name;
    semver::version version;
    std::optional<std::string> website = std::nullopt;
    std::optional<std::string> description = std::nullopt;
    std::optional<std::string> author = std::nullopt;
    std::optional<std::vector<PluginDependency>> dependencies = std::nullopt;
    std::optional<std::vector<PluginDependency>> optional_dependencies = std::nullopt;
    std::optional<std::vector<PluginDependency>> conflicts = std::nullopt;
    std::optional<std::vector<PluginDependency>> load_before = std::nullopt;

    auto operator==(PluginMetadata const &that) const noexcept -> bool
    {
        return this->id == that.id;
    }
};

struct PluginMetadataHash
{
    auto operator()(helium::plugins::PluginMetadata const &metadata) const noexcept -> std::size_t
    {
        return std::hash<std::string>()(metadata.id);
    }
};
} // namespace helium::plugins