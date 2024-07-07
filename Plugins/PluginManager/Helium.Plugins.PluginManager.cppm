/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <memory>
#include <unordered_map>

#include <pybind11/embed.h>

export module Helium.Plugins.PluginManager;

import Helium.Base;
import Helium.Plugins.PluginMetadata;
import Helium.Plugins.PluginInstance;
import Helium.Logger;

namespace py = pybind11;

namespace helium::plugins
{
auto manager_logger = logger::SharedLogger::getSharedLogger("Plugins", "PluginManager");
}

export namespace helium::plugins
{
class PluginManager : public base::HeliumObject
{
private:
    std::unordered_map<PluginMetadata, std::shared_ptr<PluginInstance>, PluginMetadata> module_map;

public:
    PluginManager()
    {
        py::initialize_interpreter();
        manager_logger->info("Python {} interpreter initialized.", PY_VERSION);
    }

    ~PluginManager()
    {
        py::finalize_interpreter();
        manager_logger->info("Python {} interpreter finalized.", PY_VERSION);
    }
};
} // namespace helium::plugins
