/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

export module Helium.BuiltinCommands.Implementations.Config;

import Helium.Config;
import Helium.Commands.CommandContext;
import Helium.Logger;
import Helium.Utils.RText;
import Helium.BuiltinCommands.Implementations.Utils;

namespace helium::commands
{
auto config_logger = logger::SharedLogger::getSharedLogger("BuiltinCommands", "Config");
}

export namespace helium::commands
{
auto configLoad(CommandContext const &ctx) -> void
{
    using namespace utils::rtext;
    sendMessageTo(ctx, config_logger, logger::LogLevel::info, "Loading config from file");
    if (not config::HeliumConfig::readConfig())
    {
        sendMessageTo(ctx, config_logger, logger::LogLevel::error, "Config loading failed");
    }
    sendMessageTo(ctx, config_logger, logger::LogLevel::info, "Config loaded");
}
auto configSave(CommandContext const &ctx) -> void
{
    using namespace utils::rtext;
    sendMessageTo(ctx, config_logger, logger::LogLevel::info, "Saving config to file");
    if (not config::HeliumConfig::saveConfig())
    {
        sendMessageTo(ctx, config_logger, logger::LogLevel::error, "Config saving failed");
    }
    sendMessageTo(ctx, config_logger, logger::LogLevel::info, "Config saved");
}
} // namespace helium::commands