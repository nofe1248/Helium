/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <filesystem>

#include <rfl.hpp>
#include <rfl/toml.hpp>

export module Helium.Config;

import Helium.Logger;

namespace helium::config
{
auto config_logger = logger::SharedLogger::getSharedLogger("Config", "Config");
}

export namespace helium::config
{
enum class ServerType
{
    VANILLA,
    BETA18,
    BUKKIT_LEGACY,
    BUKKIT,
    FORGE,
    CAT_SERVER,
    ARCLIGHT,
    BUNGEECORD,
    WATERFALL,
    VELOCITY
};
struct ServerRCONConfig
{
    bool enable = false;
    std::string address;
    int port;
    std::string password;
};
struct ServerConfig
{
    std::string path = "./server";
    ServerType type = ServerType::VANILLA;
    std::string startup_command = "java -jar server.jar";
    ServerRCONConfig rcon;
};
struct DebugConfig
{
    bool debug_mode = false;
};
struct PythonPluginConfig
{
    std::string path = "./plugins/python";
};
struct HeliumConfig
{
    PythonPluginConfig python_plugin;
    ServerConfig server;
    DebugConfig debug;
};
HeliumConfig config{};

auto readConfig() -> bool
{
    config_logger->info("Reading config from file");
    if (not std::filesystem::exists("./helium_config.toml"))
    {
        config_logger->warn("Config file not found, creating with default value.");
        auto const result = rfl::toml::save("./helium_config.toml", config);
        if(not result)
        {
            config_logger->error("Failed to create config file.");
            return false;
        }
    }
    rfl::Result<HeliumConfig> const result = rfl::toml::load<HeliumConfig>("./helium_config.toml");
    if (result)
    {
        config = result.value();
    }
    else
    {
        config_logger->error("Failed to load config file, using default config values.");
        return false;
    }
    return true;
}
auto saveConfig() -> bool
{
    config_logger->info("Saving config to file");
    auto const result = rfl::toml::save("./helium_config.toml", config);
    if(not result)
    {
        config_logger->error("Failed to save config file.");
    }
    config_logger->info("Config saved");
    return true;
}
} // namespace helium::config