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

#define FWD(x) ::std::forward<decltype(x)>(x)

export module Helium.Server.ServerInstance;

import Helium.Base.HeliumObject;
import Helium.Config;
import Helium.Events;
import Helium.Logger;

namespace asio = boost::asio;
namespace fs = std::filesystem;
namespace process = boost::process::v2;
namespace bstalgo = boost::algorithm;

namespace helium::server
{
auto server_logger = logger::SharedLogger::getSharedLogger("Server", "ServerInstance");
auto server_output_logger = logger::SharedLogger::getSharedLogger("Server", "ServerOutput", true);
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
    std::shared_ptr<process::process> server_process_ptr_;
    std::thread output_processing_thread_;

    fs::path server_path_;
    config::ServerType server_type_;
    std::string server_startup_executable_;
    std::vector<std::string> server_startup_parameters_;

    asio::readable_pipe server_stdout_pipe_;
    asio::readable_pipe server_stderr_pipe_;
    asio::writable_pipe server_stdin_pipe_;

    ServerState server_state_;

public:
    explicit ServerInstance()
        : io_context_(), server_process_ptr_(), server_path_(config::config.server.path), server_type_(config::config.server.type),
          server_startup_executable_(config::config.server.startup_command_executable),
          server_startup_parameters_(config::config.server.startup_command_parameters), server_stdout_pipe_(this->io_context_),
          server_stderr_pipe_(this->io_context_), server_stdin_pipe_(this->io_context_), server_state_(ServerState::SERVER_STATE_UNINITIALIZED)
    {
        this->server_state_ = ServerState::SERVER_STATE_STOPPED;
        this->server_state_ = ServerState::SERVER_STATE_STARTING;

        if (not fs::path{this->server_startup_executable_}.is_absolute())
        {
            this->server_startup_executable_ = process::environment::find_executable(this->server_startup_executable_).string();
        }

        this->server_path_ = fs::absolute(this->server_path_);

        this->server_process_ptr_ = std::make_shared<process::process>(
            process::process(this->io_context_, this->server_startup_executable_, this->server_startup_parameters_,
                             process::process_stdio{this->server_stdin_pipe_, this->server_stdout_pipe_, this->server_stderr_pipe_},
                             process::process_start_dir{this->server_path_.string()}));
        this->server_state_ = ServerState::SERVER_STATE_RUNNING;
        this->output_processing_thread_ = std::move(std::thread{[this] {
            server_logger->info("Server output processing thread started");
            boost::system::error_code ec;
            std::string output_buffer;
            std::vector<std::string> output_lines;
            while (this->server_process_ptr_->running())
            {
                this->server_stdout_pipe_.read_some(asio::buffer(output_buffer), ec);
                assert(ec == asio::eof);
                if (output_buffer.empty())
                {
                    continue;
                }
                bstalgo::trim(output_buffer);
                boost::split(output_lines, output_buffer, boost::is_any_of("\n"), boost::token_compress_on);
                for (auto const &line : output_lines)
                {
                    server_output_logger->log_raw(logger::LogLevel::info, line);
                }
            }
            server_logger->info("Server output processing thread stopping");
        }});
        this->output_processing_thread_.detach();
    }

    ~ServerInstance()
    {
        //this->stop();
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
} // namespace helium::server