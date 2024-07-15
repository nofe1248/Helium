/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <filesystem>
#include <iostream>
#include <memory>
#include <ranges>
#include <string>
#include <unordered_map>

#include <graaflib/algorithm/topological_sorting/dfs_topological_sorting.tpp>
#include <graaflib/graph.h>

#include <pybind11/embed.h>

#include <semver.hpp>

#include <cpptrace/cpptrace.hpp>

export module Helium.Plugins.PluginManager;

import Helium.Base;
import Helium.Config;
import Helium.Plugins.PluginMetadata;
import Helium.Plugins.PluginInstance;
import Helium.Logger;

namespace py = pybind11;
namespace fs = std::filesystem;

namespace helium::plugins
{
auto manager_logger = logger::SharedLogger::getSharedLogger("Plugins", "PluginManager");
}

export namespace helium::plugins
{
class PluginManager : public base::HeliumObject
{
private:
    std::unordered_map<std::string, std::shared_ptr<PluginInstance>> plugin_map_;
    fs::path plugin_path_;

public:
    PluginManager()
    {
        py::initialize_interpreter();
        this->plugin_path_ = fs::absolute(fs::path{config::config.python_plugin.path});
        if (not fs::exists(this->plugin_path_))
        {
            fs::create_directories(this->plugin_path_);
        }
        py::module_ module_sys_ = py::module_::import("sys");
        module_sys_.attr("path").cast<py::list>().append(this->plugin_path_.string());
        manager_logger->info("Python {} interpreter initialized.", PY_VERSION);
    }

    ~PluginManager()
    {
        for (auto &plugin_ptr : this->plugin_map_ | std::views::values)
        {
            plugin_ptr.reset();
        }
        py::finalize_interpreter();
        manager_logger->info("Python {} interpreter finalized.", PY_VERSION);
    }

    auto SearchPlugins() const -> std::vector<fs::path>
    {
        std::vector<fs::path> plugin_paths;
        for (auto const &entry : fs::recursive_directory_iterator(this->plugin_path_))
        {
            if (entry.path().extension() == ".py")
            {
                plugin_paths.push_back(entry.path());
            }
        }
        return plugin_paths;
    }

    auto RegisterPlugin(fs::path const &plugin_path) -> bool
    {
        if (not fs::exists(plugin_path))
        {
            manager_logger->error("Plugin {} not found.", plugin_path.string());
            return false;
        }
        try
        {
            auto plugin_ptr = std::make_shared<PluginInstance>(plugin_path);
            std::string plugin_id = plugin_ptr->getMetadata().id;
            if (not this->plugin_map_.contains(plugin_id))
            {
                this->plugin_map_[plugin_id] = plugin_ptr;
            }
            else
            {
                manager_logger->critical("Plugin {} has already registered", plugin_id);
                return false;
            }
        }
        catch (py::error_already_set const &py_error)
        {
            manager_logger->error("Plugin {} failed to register due to exception : {}", plugin_path.string(), py_error.what());
            return false;
        }
        catch (cpptrace::exception const &exception)
        {
            manager_logger->error("Plugin {} failed to register due to exception : {}", plugin_path.string(), exception.what());
            manager_logger->error("Exception stacktrace :");
            exception.trace().print_with_snippets(std::cerr, true);
        }
        catch (std::exception const &exception)
        {
            manager_logger->error("Plugin {} failed to register due to exception : {}", plugin_path.string(), exception.what());
            return false;
        }
        catch (...)
        {
            manager_logger->error("Plugin {} failed to register due to unknown exception", plugin_path.string());
            return false;
        }
        return true;
    }

    auto LoadAllPlugins() -> void
    {
        manager_logger->info("Calculating plugin dependencies...");
        bool can_load = true;
        for (auto const &plugin : this->plugin_map_ | std::views::values)
        {
            auto &metadata_ref = plugin->getMetadataReference();
            if (metadata_ref.conflicts.has_value())
            {
                for (auto &conflict : metadata_ref.conflicts.value())
                {
                    if (this->plugin_map_.contains(conflict.id) and
                        not semver::range::satisfies(this->plugin_map_.at(conflict.id)->getMetadataReference().version, conflict.version_range,
                                                     semver::range::satisfies_option::include_prerelease))
                    {
                        auto &conflict_metadata_ref = this->plugin_map_.at(conflict.id)->getMetadataReference();
                        manager_logger->critical("Plugin {} {} conflicts with plugin {} {}", metadata_ref.id, metadata_ref.version.to_string(),
                                                 conflict_metadata_ref.id, metadata_ref.version.to_string());
                        can_load = false;
                    }
                }
            }
        }

        std::vector<std::shared_ptr<PluginInstance>> load_sequence;
        graaf::directed_graph<std::shared_ptr<PluginInstance>, int> dependency_graph;
        std::unordered_map<std::size_t, std::shared_ptr<PluginInstance>> vertex_id_to_plugin_instance_map;
        std::unordered_map<std::string, std::size_t> plugin_id_to_vertex_id_map;

        for (auto const &plugin : this->plugin_map_ | std::views::values)
        {
            auto vertex_id = dependency_graph.add_vertex(plugin);
            vertex_id_to_plugin_instance_map[vertex_id] = plugin;
            plugin_id_to_vertex_id_map[plugin->getMetadataReference().id] = vertex_id;
        }

        for (auto const &plugin : this->plugin_map_ | std::views::values)
        {
            auto &metadata_ref = plugin->getMetadataReference();
            if (metadata_ref.dependencies.has_value())
            {
                for (auto &dependency : metadata_ref.dependencies.value())
                {
                    if (dependency.id == "Helium")
                    {
                        if (not semver::range::satisfies(base::helium_version, dependency.version_range,
                                                         semver::range::satisfies_option::include_prerelease))
                        {
                            manager_logger->critical("Dependency unsatisfied : Plugin {} {} requires Helium {}, but only found Helium {}",
                                                     metadata_ref.id, metadata_ref.version.to_string(), dependency.version_range,
                                                     base::helium_version.to_string());
                            can_load = false;
                        }
                        continue;
                    }
                    if (not this->plugin_map_.contains(dependency.id))
                    {
                        manager_logger->critical("Dependency unsatisfied : Plugin {} {} requires {} {}", metadata_ref.id,
                                                 metadata_ref.version.to_string(), dependency.id, dependency.version_range);
                        can_load = false;
                        continue;
                    }
                    if (not semver::range::satisfies(this->plugin_map_.at(dependency.id)->getMetadataReference().version, dependency.version_range,
                                                     semver::range::satisfies_option::include_prerelease))
                    {
                        manager_logger->critical("Dependency unsatisfied : Plugin {} {} requires {} {}, but only found {}", metadata_ref.id,
                                                 metadata_ref.version.to_string(), dependency.id, dependency.version_range,
                                                 this->plugin_map_.at(dependency.id)->getMetadataReference().version.to_string());
                        can_load = false;
                        continue;
                    }
                    auto dependency_id = plugin_id_to_vertex_id_map.at(dependency.id);
                    auto plugin_id = plugin_id_to_vertex_id_map.at(metadata_ref.id);
                    dependency_graph.add_edge(dependency_id, plugin_id, 1);
                }
            }
            if (metadata_ref.optional_dependencies.has_value())
            {
                for (auto &optional_dependency : metadata_ref.optional_dependencies.value())
                {
                    if (optional_dependency.id == "Helium")
                    {
                        if (not semver::range::satisfies(base::helium_version, optional_dependency.version_range,
                                                         semver::range::satisfies_option::include_prerelease))
                        {
                            manager_logger->warn("Optional dependency unsatisfied : Plugin {} {} requires Helium {}, but only found Helium {}",
                                                 metadata_ref.id, metadata_ref.version.to_string(), optional_dependency.version_range,
                                                 base::helium_version.to_string());
                        }
                        continue;
                    }
                    if (not this->plugin_map_.contains(optional_dependency.id))
                    {
                        manager_logger->warn("Optional dependency unsatisfied : Plugin {} {} requires {} {}", metadata_ref.id,
                                             metadata_ref.version.to_string(), optional_dependency.id, optional_dependency.version_range);
                        continue;
                    }
                    if (not semver::range::satisfies(this->plugin_map_.at(optional_dependency.id)->getMetadataReference().version,
                                                     optional_dependency.version_range, semver::range::satisfies_option::include_prerelease))
                    {
                        manager_logger->warn("Optional dependency unsatisfied : Plugin {} {} requires {} {}, but only found {}", metadata_ref.id,
                                             metadata_ref.version.to_string(), optional_dependency.id, optional_dependency.version_range,
                                             this->plugin_map_.at(optional_dependency.id)->getMetadataReference().version.to_string());
                        continue;
                    }
                    auto optional_dependency_id = plugin_id_to_vertex_id_map.at(optional_dependency.id);
                    auto plugin_id = plugin_id_to_vertex_id_map.at(metadata_ref.id);
                    dependency_graph.add_edge(optional_dependency_id, plugin_id, 1);
                }
            }
        }

        if (auto topological_sort_result_opt = graaf::algorithm::dfs_topological_sort(dependency_graph))
        {
            for (auto const &vertex_id : topological_sort_result_opt.value())
            {
                if (can_load)
                {
                    vertex_id_to_plugin_instance_map.at(vertex_id)->load();
                }
            }
        }
        else
        {
            manager_logger->critical("Plugin loading failed due to cycles in dependency graph.");
            can_load = false;
        }
    }

    auto SearchAndLoadAllPlugins() -> void
    {
        for (auto const &plugin_path : this->SearchPlugins())
        {
            if (not this->RegisterPlugin(plugin_path))
            {
                manager_logger->critical("Plugin {} failed to register", plugin_path.string());
                return;
            }
        }
        this->LoadAllPlugins();
        manager_logger->flush();
    }
};
} // namespace helium::plugins
