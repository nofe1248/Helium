/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <mutex>
#include <stop_token>
#include <string>
#include <thread>
#include <vector>

export module Helium.Commands.CommandExecutionThread;

import Helium.Base.HeliumObject;
import Helium.Commands.CommandSource;
import Helium.Commands.CommandDispatcher;
import Helium.Logger;

namespace helium::commands
{
auto logger = logger::SharedLogger::getSharedLogger("Commands", "CommandExecutionThread");
}

export namespace helium::commands
{
class CommandExecutionThread final : public base::HeliumObject
{
private:
    struct CommandInfo
    {
        std::string command;
        CommandSource source;
    };
    std::vector<CommandInfo> commands_queue_{};
    std::jthread thread_{};
    mutable std::mutex mutex_{};

    auto mainCommandExecutionThreadLoop(std::stop_token st) -> void
    {
        logger->info("Command execution thread started");
        while (not st.stop_requested())
        {
            std::vector<CommandInfo> commands{};
            {
                std::unique_lock<std::mutex> lock(mutex_);
                std::swap(commands, this->commands_queue_);
            }
            for (auto const &command : commands)
            {
                if (not CommandDispatcher::getInstance().tryExecuteCommand(command.source, command.command))
                {
                    logger->error("Command {} failed to execute", command.command);
                }
            }
        }
        logger->info("Command execution thread stopping");
    }

public:
    static auto getInstance() noexcept -> CommandExecutionThread &
    {
        static CommandExecutionThread instance;
        return instance;
    }

    auto run() -> void
    {
        this->thread_ = std::move(std::jthread(&CommandExecutionThread::mainCommandExecutionThreadLoop, this));
    }

    auto stop() -> void
    {
        this->thread_.request_stop();
        if (this->thread_.joinable())
        {
            this->thread_.join();
        }
    }

    ~CommandExecutionThread()
    {
        this->stop();
    }

    auto addCommand(CommandSource const &source, std::string const &command) -> void
    {
        std::lock_guard lock(this->mutex_);
        this->commands_queue_.emplace_back(CommandInfo{.command = command, .source = source});
    }
};
} // namespace helium::commands