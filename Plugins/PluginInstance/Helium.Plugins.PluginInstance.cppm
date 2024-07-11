/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <filesystem>
#include <format>
#include <memory>
#include <string>

#include <pybind11/embed.h>

#include <semver.hpp>

export module Helium.Plugins.PluginInstance;

import Helium.Base;
import Helium.Logger;
import Helium.Plugins.PluginMetadata;

namespace py = pybind11;
namespace fs = std::filesystem;

namespace helium::plugins
{
auto instance_logger = logger::SharedLogger::getSharedLogger("Plugins", "PluginInstance");
}

export namespace helium::plugins
{
class PluginInstance : public base::HeliumObject
{
private:
    fs::path plugin_path_;
    py::module_ plugin_module_;
    PluginMetadata metadata_;

public:
    explicit PluginInstance(fs::path plugin_path) : plugin_path_(std::move(plugin_path))
    {
        instance_logger->info("Initializing plugin {}", plugin_path_.string());
        std::string module_name = this->plugin_path_.filename().replace_extension("").string();
        this->plugin_module_ = py::module_::import(module_name.c_str());
        if (not hasattr(this->plugin_module_, "plugin_metadata"))
        {
            throw std::runtime_error(std::format("Plugin {} metadata not found", module_name));
        }
        auto metadata = this->plugin_module_.attr("plugin_metadata").cast<py::dict>();
        if (not metadata.contains("id"))
        {
            throw std::runtime_error(std::format("Plugin {} metadata must contain \"id\" field", module_name));
        }
        if (not metadata.contains("name"))
        {
            throw std::runtime_error(std::format("Plugin {} metadata must contain \"name\" field", module_name));
        }
        if (not metadata.contains("version"))
        {
            throw std::runtime_error(std::format("Plugin {} metadata must contain \"version\" field", module_name));
        }
        this->metadata_.plugin_path = this->plugin_path_;
        this->metadata_.id = metadata["id"].cast<std::string>();
        this->metadata_.name = metadata["name"].cast<std::string>();
        this->metadata_.version = semver::version{metadata["version"].cast<std::string>()};
        if (metadata.contains("website"))
        {
            this->metadata_.description = metadata["website"].cast<std::string>();
        }
        if (metadata.contains("description"))
        {
            this->metadata_.description = metadata["description"].cast<std::string>();
        }
        if (metadata.contains("author"))
        {
            this->metadata_.description = metadata["author"].cast<std::string>();
        }
        if (metadata.contains("dependencies"))
        {
            auto dependencies = metadata["dependencies"].cast<py::list>();
            this->metadata_.dependencies = std::vector<PluginMetadata::PluginDependency>{};
            for (auto const &dependency : dependencies)
            {
                this->metadata_.dependencies.value().push_back(
                    PluginMetadata::PluginDependency{.id = dependency["id"].cast<std::string>(),
                                                     .version_range = dependency["version"].cast<std::string>()});
            }
        }
        if (metadata.contains("optional_dependencies"))
        {
            auto optional_dependencies = metadata["optional_dependencies"].cast<py::list>();
            this->metadata_.optional_dependencies = std::vector<PluginMetadata::PluginDependency>{};
            for (auto const &dependency : optional_dependencies)
            {
                this->metadata_.optional_dependencies.value().push_back(
                    PluginMetadata::PluginDependency{.id = dependency["id"].cast<std::string>(),
                                                     .version_range = dependency["version"].cast<std::string>()});
            }
        }
        if (metadata.contains("conflicts"))
        {
            auto conflicts = metadata["conflicts"].cast<py::list>();
            this->metadata_.conflicts = std::vector<PluginMetadata::PluginDependency>{};
            for (auto const &dependency : conflicts)
            {
                this->metadata_.conflicts.value().push_back(
                    PluginMetadata::PluginDependency{.id = dependency["id"].cast<std::string>(),
                                                     .version_range = dependency["version"].cast<std::string>()});
            }
        }
    }

    auto load() -> void
    {
        instance_logger->info("Loading plugin {}", this->metadata_.id);
        instance_logger->flush();
        if (hasattr(this->plugin_module_, "on_load"))
        {
            (void)this->plugin_module_.attr("on_load")();
        }
    }

    auto unload() -> void
    {
        instance_logger->info("Unloading plugin {}", this->metadata_.id);
        if (hasattr(this->plugin_module_, "on_unload"))
        {
            (void)this->plugin_module_.attr("on_unload")();
        }
    }

    auto reload() -> void
    {
        instance_logger->info("Reloading plugin {}", this->metadata_.id);
        if (hasattr(this->plugin_module_, "on_reload"))
        {
            (void)this->plugin_module_.attr("on_reload")();
        }
    }

    ~PluginInstance()
    {
        this->unload();
    }

    auto getMetadata() -> PluginMetadata
    {
        return this->metadata_;
    }

    auto getMetadataReference() -> PluginMetadata &
    {
        return this->metadata_;
    }
};
} // namespace helium::plugins