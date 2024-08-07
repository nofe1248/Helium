/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <stop_token>
#include <thread>

#include <plf_hive.h>

export module Helium.Server.ServerOutputProcessThread;

import Helium.Base.HeliumObject;
import Helium.Server.ServerOutputParser.ServerOutputInfo;

export namespace helium::server
{
class ServerOutputProcessThread final : public base::HeliumObject
{
private:
    plf::hive<ServerOutputInfo> info_queue_;
    std::jthread thread_;

    auto mainServerOutputProcessLoop(std::stop_token st) -> void
    {
        while (not st.stop_requested())
        {
            for (auto const &output_info : this->info_queue_)
            {
                if (output_info.info_type == ServerOutputInfoType::RCON_STARTED)
                {
                }
                else if (output_info.info_type == ServerOutputInfoType::SERVER_STARTED)
                {
                }
                else if (output_info.info_type == ServerOutputInfoType::SERVER_STOPPING)
                {
                }
                else if (output_info.info_type == ServerOutputInfoType::SERVER_ADDRESS)
                {
                }
                else if (output_info.info_type == ServerOutputInfoType::SERVER_VERSION)
                {
                }
                else if (output_info.info_type == ServerOutputInfoType::SERVER_OUTPUT)
                {
                }
                else if (output_info.info_type == ServerOutputInfoType::PLAYER_MESSAGE)
                {
                }
                else if (output_info.info_type == ServerOutputInfoType::PLAYER_JOINED)
                {
                }
                else if (output_info.info_type == ServerOutputInfoType::PLAYER_LEFT)
                {
                }
            }
        }
    }

public:
    ServerOutputProcessThread() : info_queue_(), thread_(std::move(std::jthread(&ServerOutputProcessThread::mainServerOutputProcessLoop, this)))
    {
    }

    ~ServerOutputProcessThread()
    {
        this->thread_.request_stop();
        this->thread_.join();
    }

    auto addServerOutputInfo(ServerOutputInfo const &output_info) -> void
    {
        this->info_queue_.insert(output_info);
    }
};
ServerOutputProcessThread server_output_process_thread;
} // namespace helium::server