/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <memory>
#include <unordered_map>

export module Helium.Plugins.PluginManager;

import Helium.Base;
import Helium.Plugins.PluginMetadata;
import Helium.Plugins.PluginInstance;

export namespace helium::plugins {
    class PluginManager : public base::HeliumObject {
    private:
        std::unordered_map<PluginMetadata, std::shared_ptr<PluginInstance>, PluginMetadata> module_map;

    public:
        [[nodiscard]] static auto getInstance() -> PluginManager & {
            static PluginManager instance;
            return instance;
        }
    };
} // namespace helium::modules
