/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <filesystem>
#include <string>

#include <boost/asio.hpp>
#include <boost/process/v2.hpp>

#include <boost/sml2>

export module Helium.Server.ServerInstance;

import Helium.Base.HeliumObject;
import Helium.Config;
import Helium.Events;

namespace asio = boost::asio;
namespace fs = std::filesystem;
namespace process = boost::process::v2;

export namespace helium::server
{
class ServerInstance final : public base::HeliumObject
{
private:
    asio::io_context io_context_;
    std::shared_ptr<process::process> server_process_ptr_;

    fs::path server_path_;
    config::ServerType server_type_;
    std::string server_startup_command_;

    asio::readable_pipe server_stdout_pipe_;
    asio::readable_pipe server_stderr_pipe_;
    asio::writable_pipe server_stdin_pipe_;

    struct ServerInitialize
    {
    };
    struct ServerStarted
    {
    };
    struct ServerFinalize
    {
    };
    struct ServerStopped
    {
    };
    struct ServerPause
    {
    };
    struct ServerResume
    {
    };

public:
    explicit ServerInstance()
        : io_context_(), server_process_ptr_(), server_path_(config::config.server.path), server_type_(config::config.server.type),
          server_startup_command_(config::config.server.startup_command), server_stdout_pipe_(this->io_context_),
          server_stderr_pipe_(this->io_context_), server_stdin_pipe_(this->io_context_)
    {
        this->server_process_ptr_ = std::make_shared<process::process>(std::move(process::process(
            this->io_context_, "", {}, process::process_stdio{this->server_stdin_pipe_, this->server_stdout_pipe_, this->server_stderr_pipe_},
            process::process_start_dir{this->server_path_.string()})));
    }

    ~ServerInstance()
    {
    }
};
} // namespace helium::server