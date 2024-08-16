/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

export module Helium.BuiltinCommands.Implementations.Debug;

import Helium.Commands.CommandContext;
import Helium.Logger;
import Helium.BuiltinCommands.Implementations.Utils;

namespace helium::commands
{
auto debug_logger = logger::SharedLogger::getSharedLogger("BuiltinCommands", "Debug");
}

export namespace helium::commands
{
auto heliumDebug(CommandContext const &ctx, bool enable) -> void
{
    if (enable)
    {
        sendMessageTo(ctx, debug_logger, logger::LogLevel::info, "Debug mode enabled");
        logger::LoggerImpl::enableAllDebugLog();
    }
    else
    {
        sendMessageTo(ctx, debug_logger, logger::LogLevel::info, "Debug mode disabled");
        logger::LoggerImpl::disableAllDebugLog();
    }
}
} // namespace helium::commands