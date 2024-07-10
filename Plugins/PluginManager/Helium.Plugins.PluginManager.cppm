/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <filesystem>
#include <memory>
#include <string>
#include <ranges>
#include <unordered_map>

#include <pybind11/embed.h>

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
        for(auto &plugin_ptr : this->plugin_map_ | std::views::values)
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

    auto LoadPlugin(fs::path const &plugin_path) -> bool
    {
        if (not fs::exists(plugin_path))
        {
            manager_logger->error("Plugin {} not found.", plugin_path.string());
            return false;
        }
        try
        {
            auto plugin_ptr = std::make_shared<PluginInstance>(plugin_path);
            this->plugin_map_[plugin_ptr->getMetadata().id] = plugin_ptr;
        }
        catch (py::error_already_set const &py_error)
        {
            manager_logger->error("Plugin {} failed to load due to exception : {}.", plugin_path.string(), py_error.what());
            return false;
        }
        catch (std::exception const &exception)
        {
            manager_logger->error("Plugin {} failed to load due to exception : {}.", plugin_path.string(), exception.what());
            return false;
        }
        catch (...)
        {
            manager_logger->error("Plugin {} failed to load due to unknown exception.", plugin_path.string());
            return false;
        }
        return true;
    }

    auto SearchAndLoadAllPlugins() -> void
    {
        for (auto const &plugin_path : this->SearchPlugins())
        {
            this->LoadPlugin(plugin_path);
        }
    }
};
} // namespace helium::plugins
