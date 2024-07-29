/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <chrono>
#include <filesystem>
#include <string>
#include <thread>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/process/v2.hpp>

#include <boost/sml2>

#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#define FWD(x) ::std::forward<decltype(x)>(x)

export module Helium.Server.ServerInstance;

import Helium.Base.HeliumObject;
import Helium.Config;
import Helium.Events;
import Helium.Logger;
import Helium.Server.ServerOutputParser;

namespace asio = boost::asio;
namespace fs = std::filesystem;
namespace process = boost::process::v2;
namespace bstalgo = boost::algorithm;

namespace helium::server
{
auto server_logger = logger::SharedLogger::getSharedLogger("Server", "ServerInstance");
} // namespace helium::server

export namespace helium::server
{
enum class ServerState
{
    SERVER_STATE_UNINITIALIZED,
    SERVER_STATE_STARTING,
    SERVER_STATE_RUNNING,
    SERVER_STATE_SUSPENDED,
    SERVER_STATE_STOPPING,
    SERVER_STATE_STOPPED
};

class ServerInstance final : public base::HeliumObject
{
private:
    asio::io_context io_context_;
    std::shared_ptr<process::popen> server_process_ptr_;
    std::thread output_processing_thread_;

    fs::path server_path_;
    config::ServerType server_type_;
    std::string server_startup_executable_;
    std::vector<std::string> server_startup_parameters_;

    ServerState server_state_;

public:
    explicit ServerInstance()
        : io_context_(), server_process_ptr_(), server_path_(config::config.server.path), server_type_(config::config.server.type),
          server_startup_executable_(config::config.server.startup_command_executable),
          server_startup_parameters_(config::config.server.startup_command_parameters), server_state_(ServerState::SERVER_STATE_UNINITIALIZED)
    {
        this->server_state_ = ServerState::SERVER_STATE_STOPPED;
        this->server_state_ = ServerState::SERVER_STATE_STARTING;

        if (not fs::path{this->server_startup_executable_}.is_absolute())
        {
            this->server_startup_executable_ = process::environment::find_executable(this->server_startup_executable_).string();
            server_logger->info("Server startup executable path is not absolute, found probable executable path is {}",
                                this->server_startup_executable_);
        }

        if (not this->server_path_.is_absolute())
        {
            this->server_path_ = fs::absolute(this->server_path_);
        }

        server_logger->info("Launching server");
        this->server_process_ptr_ =
            std::make_shared<process::popen>(this->io_context_, this->server_startup_executable_, this->server_startup_parameters_,
                                             process::process_start_dir{this->server_path_.string()});

        if (not this->server_process_ptr_->is_open())
        {
            throw std::runtime_error("Failed to open server process");
        }

        if (not this->server_process_ptr_->running())
        {
            throw std::runtime_error("Failed to launch server process");
        }

        server_logger->info("Server launchered with Process ID {}", this->server_process_ptr_->id());

        this->server_state_ = ServerState::SERVER_STATE_RUNNING;
        this->output_processing_thread_ = std::move(std::thread{[this] {
            server_logger->info("Server output processing thread started");
            boost::system::error_code ec;
            std::string output_buffer;

            auto logger_ptr = std::make_shared<spdlog::async_logger>("helium_server_output_logger", std::make_shared<spdlog::sinks::stdout_color_sink_mt>(), spdlog::thread_pool());
            logger_ptr->set_pattern("%v");

            spdlog::register_logger(logger_ptr);

            while (this->server_process_ptr_->running())
            {
                asio::read_until(*this->server_process_ptr_, asio::dynamic_buffer(output_buffer), '\n');
                if (output_buffer.empty())
                {
                    continue;
                }
                output_buffer.erase(output_buffer.size() - 1);
                logger_ptr->info(output_buffer);
                output_buffer.clear();
            }
            server_logger->info("Server stopped with return code {}", this->server_process_ptr_->exit_code());
            server_logger->info("Server output processing thread stopping");
        }});
        this->output_processing_thread_.detach();
    }

    ~ServerInstance()
    {
        // this->stop();
    }

    auto send_raw_input(this auto &&self, std::string const &input) -> bool
    {
        if (FWD(self).server_state_ != ServerState::SERVER_STATE_RUNNING)
        {
            return false;
        }
        assert(FWD(self).server_process_ptr_->running());
        auto buffer = input;
        buffer.append("\n");
        asio::write(*FWD(self).server_process_ptr_, asio::const_buffer{buffer.data(), buffer.size()});
        return true;
    }

    auto stop(this auto &&self) -> bool
    {
    }

    auto kill(this auto &&self) -> bool
    {
        FWD(self).server_process_ptr_->terminate();
    }

    auto suspend(this auto &&self) -> bool
    {
        if (FWD(self).server_state_ != ServerState::SERVER_STATE_RUNNING)
        {
            return false;
        }
        FWD(self).server_process_ptr_->suspend();
        return true;
    }

    auto resume(this auto &&self) -> bool
    {
        if (FWD(self).server_state_ != ServerState::SERVER_STATE_SUSPENDED)
        {
            return false;
        }
        FWD(self).server_process_ptr_->resume();
        return true;
    }
};

std::shared_ptr<ServerInstance> server_instance = nullptr;
} // namespace helium::server