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
        while(not st.stop_requested())
        {

        }
    }

public:
    ServerOutputProcessThread() : info_queue_(), thread_(std::move(std::jthread(&ServerOutputProcessThread::mainServerOutputProcessLoop, this)))
    {
    }
};
} // namespace helium::server