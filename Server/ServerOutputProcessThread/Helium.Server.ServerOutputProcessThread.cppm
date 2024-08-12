/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <mutex>
#include <stop_token>
#include <thread>
#include <variant>
#include <vector>

export module Helium.Server.ServerOutputProcessThread;

import Helium.Base.HeliumObject;
import Helium.Server.ServerOutputParser.ServerOutputInfo;
import Helium.Events.EventBus;
import Helium.Events.Helium;
import Helium.Logger;
import Helium.Commands.CommandContext;
import Helium.Commands.CommandSource;
import Helium.Commands.CommandDispatcher;

namespace helium::server
{
auto output_process_thread_logger = logger::SharedLogger::getSharedLogger("Server", "OutputInfoProcessThread");
}

export namespace helium::server
{
class ServerOutputProcessThread final : public base::HeliumObject
{
private:
    std::vector<ServerOutputInfo> info_queue_;
    std::jthread thread_;
    mutable std::mutex mutex_;

    auto mainServerOutputProcessLoop(std::stop_token st) -> void
    {
        output_process_thread_logger->info("Server output info processing thread started");
        while (not st.stop_requested())
        {
            std::vector<ServerOutputInfo> process_queue{};
            {
                std::unique_lock lock(this->mutex_);
                std::swap(process_queue, this->info_queue_);
            }
            for (auto const &output_info : process_queue)
            {
                if (output_info.info_type == ServerOutputInfoType::RCON_STARTED)
                {
                    events::EventBus::getInstancePointer()->postponeEvent<events::RCONStarted>(events::RCONStarted{});
                }
                else if (output_info.info_type == ServerOutputInfoType::SERVER_STARTED)
                {
                    events::EventBus::getInstancePointer()->postponeEvent<events::ServerStarted>(events::ServerStarted{});
                }
                else if (output_info.info_type == ServerOutputInfoType::SERVER_STOPPING)
                {
                    events::EventBus::getInstancePointer()->postponeEvent<events::ServerStopping>(events::ServerStopping{});
                }
                else if (output_info.info_type == ServerOutputInfoType::SERVER_ADDRESS)
                {
                    events::EventBus::getInstancePointer()->postponeEvent<events::ServerAddress>(
                        events::ServerAddress{.info = std::get<ServerAddress>(output_info.info.value())});
                }
                else if (output_info.info_type == ServerOutputInfoType::SERVER_VERSION)
                {
                    events::EventBus::getInstancePointer()->postponeEvent<events::ServerVersion>(
                        events::ServerVersion{.info = std::get<ServerVersion>(output_info.info.value())});
                }
                else if (output_info.info_type == ServerOutputInfoType::PLAYER_MESSAGE)
                {
                    auto message = std::get<PlayerMessage>(output_info.info.value()).player_message;
                    if (std::get<PlayerMessage>(output_info.info.value()).player_name == "Server")
                    {
                        if (message.front() == '#')
                        {
                            commands::CommandSource command_source("server", "server");
                            commands::CommandDispatcher::getInstance().tryExecuteCommand(command_source, message);
                        }
                        events::EventBus::getInstancePointer()->postponeEvent<events::ServerMessage>(events::ServerMessage{
                            .info = ServerMessage{.is_secure = std::get<PlayerMessage>(output_info.info.value()).is_secure,
                                                  .message = std::get<PlayerMessage>(output_info.info.value()).player_message}
                        });
                    }
                    else
                    {
                        if (message.front() == '#')
                        {
                            commands::CommandSource command_source("player", std::get<PlayerMessage>(output_info.info.value()).player_name);
                            commands::CommandDispatcher::getInstance().tryExecuteCommand(command_source, message);
                        }
                        events::EventBus::getInstancePointer()->postponeEvent<events::PlayerMessage>(
                            events::PlayerMessage{.info = std::get<PlayerMessage>(output_info.info.value())});
                    }
                }
                else if (output_info.info_type == ServerOutputInfoType::PLAYER_JOINED)
                {
                    events::EventBus::getInstancePointer()->postponeEvent<events::PlayerJoined>(
                        events::PlayerJoined{.info = std::get<PlayerJoined>(output_info.info.value())});
                }
                else if (output_info.info_type == ServerOutputInfoType::PLAYER_LEFT)
                {
                    events::EventBus::getInstancePointer()->postponeEvent<events::PlayerLeft>(
                        events::PlayerLeft{.info = std::get<PlayerLeft>(output_info.info.value())});
                }
                events::EventBus::getInstancePointer()->postponeEvent<events::ServerOutput>(events::ServerOutput{.info = output_info});
            }
        }
        output_process_thread_logger->info("Server output process thread stopping");
    }

public:
    static auto getInstance() noexcept -> ServerOutputProcessThread &
    {
        static ServerOutputProcessThread instance;
        return instance;
    }

    ServerOutputProcessThread() : info_queue_(), thread_()
    {
    }

    auto run() -> void
    {
        this->thread_ = std::move(std::jthread(&ServerOutputProcessThread::mainServerOutputProcessLoop, this));
    }

    auto stop() -> void
    {
        this->thread_.request_stop();
        if (this->thread_.joinable())
        {
            this->thread_.join();
        }
    }

    ~ServerOutputProcessThread()
    {
        this->stop();
    }

    auto addServerOutputInfo(ServerOutputInfo const &output_info) -> void
    {
        std::unique_lock lock(this->mutex_);
        this->info_queue_.push_back(output_info);
    }
};
} // namespace helium::server