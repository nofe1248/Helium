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
                    events::main_event_bus->postponeEvent<events::RCONStarted>(events::RCONStarted{});
                }
                else if (output_info.info_type == ServerOutputInfoType::SERVER_STARTED)
                {
                    events::main_event_bus->postponeEvent<events::ServerStarted>(events::ServerStarted{});
                }
                else if (output_info.info_type == ServerOutputInfoType::SERVER_STOPPING)
                {
                    events::main_event_bus->postponeEvent<events::ServerStopping>(events::ServerStopping{});
                }
                else if (output_info.info_type == ServerOutputInfoType::SERVER_ADDRESS)
                {
                    events::main_event_bus->postponeEvent<events::ServerAddress>(
                        events::ServerAddress{.info = std::get<ServerAddress>(output_info.info.value())});
                }
                else if (output_info.info_type == ServerOutputInfoType::SERVER_VERSION)
                {
                    events::main_event_bus->postponeEvent<events::ServerVersion>(
                        events::ServerVersion{.info = std::get<ServerVersion>(output_info.info.value())});
                }
                else if (output_info.info_type == ServerOutputInfoType::PLAYER_MESSAGE)
                {
                    events::main_event_bus->postponeEvent<events::PlayerMessage>(
                        events::PlayerMessage{.info = std::get<PlayerMessage>(output_info.info.value())});
                }
                else if (output_info.info_type == ServerOutputInfoType::PLAYER_JOINED)
                {
                    events::main_event_bus->postponeEvent<events::PlayerJoined>(
                        events::PlayerJoined{.info = std::get<PlayerJoined>(output_info.info.value())});
                }
                else if (output_info.info_type == ServerOutputInfoType::PLAYER_LEFT)
                {
                    events::main_event_bus->postponeEvent<events::PlayerLeft>(
                        events::PlayerLeft{.info = std::get<PlayerLeft>(output_info.info.value())});
                }
                events::main_event_bus->postponeEvent<events::ServerOutput>(events::ServerOutput{.info = output_info});
            }
        }
        output_process_thread_logger->info("Server output process thread stopping");
    }

public:
    ServerOutputProcessThread() : info_queue_(), thread_()
    {
    }

    auto run() -> void
    {
        this->thread_ = std::move(std::jthread(&ServerOutputProcessThread::mainServerOutputProcessLoop, this));
    }

    ~ServerOutputProcessThread()
    {
        if (this->thread_.joinable())
        {
            this->thread_.request_stop();
            this->thread_.join();
        }
    }

    auto addServerOutputInfo(ServerOutputInfo const &output_info) -> void
    {
        std::unique_lock lock(this->mutex_);
        this->info_queue_.push_back(output_info);
    }
};
ServerOutputProcessThread server_output_process_thread;
} // namespace helium::server