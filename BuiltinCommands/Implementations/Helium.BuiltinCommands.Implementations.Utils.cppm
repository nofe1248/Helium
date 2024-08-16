/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <format>
#include <string>
#include <utility>

export module Helium.BuiltinCommands.Implementations.Utils;

import Helium.Logger;
import Helium.Utils.RText;
import Helium.Commands.CommandContext;
import Helium.Server.ServerInstance;

export namespace helium::commands
{
template <typename... Args>
auto sendMessageTo(CommandContext const &ctx, logger::SharedLogger const &logger, logger::LogLevel const log_level, std::string const &fmt_str,
                   Args &&...args) -> void
{
    if (ctx.getSource().getSourceType() == std::string{"console"})
    {
        logger->log(log_level, fmt_str, std::forward<Args>(args)...);
    }
    else if (ctx.getSource().getSourceType() == std::string{"player"})
    {
        using namespace utils::rtext;
        RTextList log_head{};
        if (log_level == logger::LogLevel::trace)
        {
            log_head.addRText(RText{"[TRACE]"}.setColor(RColorClassic::white)).addRText(RText{" [Helium] "}.setColor(RColorClassic::gray));
        }
        else if (log_level == logger::LogLevel::debug)
        {
            log_head.addRText(RText{"[DEBUG]"}.setColor(RColorClassic::aqua)).addRText(RText{" [Helium] "}.setColor(RColorClassic::gray));
        }
        else if (log_level == logger::LogLevel::info)
        {
            log_head.addRText(RText{"[INFO]"}.setColor(RColorClassic::green)).addRText(RText{" [Helium] "}.setColor(RColorClassic::gray));
        }
        else if (log_level == logger::LogLevel::warn)
        {
            log_head.addRText(RText{"[WARN]"}.setColor(RColorClassic::yellow).setStyle(RStyleClassic::bold))
                .addRText(RText{" [Helium] "}.setColor(RColorClassic::gray));
        }
        else if (log_level == logger::LogLevel::error)
        {
            log_head.addRText(RText{"[ERROR]"}.setColor(RColorClassic::red).setStyle(RStyleClassic::bold))
                .addRText(RText{" [Helium] "}.setColor(RColorClassic::gray));
        }
        else if (log_level == logger::LogLevel::critical)
        {
            log_head.addRText(RText{"[CRITICAL]"}.setColor(RColorClassic::light_purple).setStyle(RStyleClassic::bold))
                .addRText(RText{" [Helium] "}.setColor(RColorClassic::gray));
        }
        else
        {
            std::unreachable();
        }
        if (sizeof...(args) > 0)
        {
            log_head.addRText(RText{std::format(std::runtime_format(fmt_str), std::forward<Args>(args)...)}.setColor(RColorClassic::white));
        }
        else
        {
            log_head.addRText(RText{fmt_str}.setColor(RColorClassic::white));
        }
        server::ServerInstance::getInstancePointer()->sendMessage(ctx.getSource().getMajorSource(), log_head);
    }
}
} // namespace helium::commands