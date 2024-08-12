/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <chrono>
#include <filesystem>
#include <stop_token>
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
import Helium.Server.ServerOutputProcessThread;
import Helium.Utils.RText;
import Helium.Events.EventBus;
import Helium.Events.Helium;

namespace asio = boost::asio;
namespace fs = std::filesystem;
namespace process = boost::process::v2;
namespace bstalgo = boost::algorithm;

using RText = helium::utils::rtext::RText;

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
    SERVER_STATE_STOPPING,
    SERVER_STATE_STOPPED
};

class ServerInstance final : public base::HeliumObject
{
private:
    asio::io_context io_context_;
    std::shared_ptr<process::popen> server_process_ptr_;
    std::unique_ptr<std::jthread> output_processing_thread_;

    fs::path server_path_;
    config::ServerType server_type_;
    std::string server_startup_executable_;
    std::vector<std::string> server_startup_parameters_;

    ServerState server_state_;

    ServerOutputParserProxy parser_;
    std::function<void()> parser_deleter_;

    auto setServerState(this auto &&self, ServerState state) -> void
    {
        FWD(self).server_state_ = state;
    }

public:
    static auto getInstancePointer() noexcept -> std::shared_ptr<ServerInstance>
    {
        static auto instance = std::make_shared<ServerInstance>();
        return instance;
    }

    explicit ServerInstance()
        : io_context_(), server_process_ptr_(), server_path_(config::HeliumConfig::getInstance().server.path),
          server_type_(config::HeliumConfig::getInstance().server.type),
          server_startup_executable_(config::HeliumConfig::getInstance().server.startup_command_executable),
          server_startup_parameters_(config::HeliumConfig::getInstance().server.startup_command_parameters),
          server_state_(ServerState::SERVER_STATE_UNINITIALIZED)
    {
        if (this->server_type_ == config::ServerType::VANILLA)
        {
            auto *parser_ptr = new VanillaServerOutputParser;
            this->parser_deleter_ = [parser_ptr] { delete parser_ptr; };
            this->parser_ = parser_ptr;
        }
        else if (this->server_type_ == config::ServerType::BETA18)
        {
            auto *parser_ptr = new Beta18ServerOutputParser;
            this->parser_deleter_ = [parser_ptr] { delete parser_ptr; };
            this->parser_ = parser_ptr;
        }
        else if (this->server_type_ == config::ServerType::BUKKIT_LEGACY)
        {
            auto *parser_ptr = new BukkitLegacyServerOutputParser;
            this->parser_deleter_ = [parser_ptr] { delete parser_ptr; };
            this->parser_ = parser_ptr;
        }
        else if (this->server_type_ == config::ServerType::BUKKIT)
        {
            auto *parser_ptr = new BukkitServerOutputParser;
            this->parser_deleter_ = [parser_ptr] { delete parser_ptr; };
            this->parser_ = parser_ptr;
        }
        else if (this->server_type_ == config::ServerType::FORGE)
        {
            auto *parser_ptr = new ForgeServerOutputParser;
            this->parser_deleter_ = [parser_ptr] { delete parser_ptr; };
            this->parser_ = parser_ptr;
        }
        else if (this->server_type_ == config::ServerType::CAT_SERVER)
        {
            auto *parser_ptr = new CatServerOutputParser;
            this->parser_deleter_ = [parser_ptr] { delete parser_ptr; };
            this->parser_ = parser_ptr;
        }
        else if (this->server_type_ == config::ServerType::ARCLIGHT)
        {
            auto *parser_ptr = new ArclightServerOutputParser;
            this->parser_deleter_ = [parser_ptr] { delete parser_ptr; };
            this->parser_ = parser_ptr;
        }
        else if (this->server_type_ == config::ServerType::BUNGEECORD)
        {
            auto *parser_ptr = new BungeecordServerOutputParser;
            this->parser_deleter_ = [parser_ptr] { delete parser_ptr; };
            this->parser_ = parser_ptr;
        }
        else if (this->server_type_ == config::ServerType::WATERFALL)
        {
            auto *parser_ptr = new WaterfallServerOutputParser;
            this->parser_deleter_ = [parser_ptr] { delete parser_ptr; };
            this->parser_ = parser_ptr;
        }
        else if (this->server_type_ == config::ServerType::VELOCITY)
        {
            auto *parser_ptr = new VelocityServerOutputParser;
            this->parser_deleter_ = [parser_ptr] { delete parser_ptr; };
            this->parser_ = parser_ptr;
        }
        else if (this->server_type_ == config::ServerType::CUSTOM)
        {
            auto *parser_ptr = new PythonCustomParserWrapper;
            this->parser_deleter_ = [parser_ptr] { delete parser_ptr; };
            this->parser_ = parser_ptr;
        }
        else
        {
            std::unreachable();
        }

        if (not fs::path{this->server_startup_executable_}.is_absolute())
        {
            this->server_startup_executable_ = process::environment::find_executable(this->server_startup_executable_).string();
            server_logger->info("Server startup executable path is not absolute, found probable executable path : {}",
                                this->server_startup_executable_);
        }

        if (not this->server_path_.is_absolute())
        {
            this->server_path_ = fs::absolute(this->server_path_);
        }

        this->server_state_ = ServerState::SERVER_STATE_STOPPED;
    }

    ~ServerInstance()
    {
        this->stop();
        if (this->output_processing_thread_)
        {
            if (this->output_processing_thread_->joinable())
            {
                this->output_processing_thread_->request_stop();
                this->output_processing_thread_->join();
            }
            this->output_processing_thread_.reset();
        }
        this->parser_deleter_();
    }

    auto sendRawInput(this auto &&self, std::string const &input) -> bool
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

    auto start(this auto &&self) -> bool
    {
        if (FWD(self).server_state_ != ServerState::SERVER_STATE_STOPPED)
        {
            return false;
        }

        FWD(self).server_state_ = ServerState::SERVER_STATE_STARTING;

        if (FWD(self).output_processing_thread_)
        {
            FWD(self).output_processing_thread_->request_stop();
            if (FWD(self).output_processing_thread_->joinable())
            {
                FWD(self).output_processing_thread_->join();
            }
            FWD(self).output_processing_thread_.reset();
        }
        if (FWD(self).server_process_ptr_)
        {
            FWD(self).server_process_ptr_.reset();
        }

        server_logger->info("Launching server");
        events::EventBus::getInstancePointer()->postponeEvent<events::ServerStarting>(events::ServerStarting{});

        FWD(self).server_process_ptr_ =
            std::make_shared<process::popen>(FWD(self).io_context_, FWD(self).server_startup_executable_, FWD(self).server_startup_parameters_,
                                             process::process_start_dir{FWD(self).server_path_.string()});

        if (not FWD(self).server_process_ptr_->is_open())
        {
            throw std::runtime_error("Failed to open server process");
        }

        if (not FWD(self).server_process_ptr_->running())
        {
            throw std::runtime_error("Failed to launch server process");
        }

        server_logger->info("Server launchered with Process ID {}", FWD(self).server_process_ptr_->id());

        FWD(self).server_state_ = ServerState::SERVER_STATE_RUNNING;
        FWD(self).output_processing_thread_ = std::make_unique<std::jthread>([&self](std::stop_token st) {
            server_logger->info("Server output processing thread started");
            boost::system::error_code ec;
            std::string output_buffer;
            auto server_popen = self.server_process_ptr_;

            auto logger_ptr = std::make_shared<spdlog::async_logger>("helium_server_output_logger",
                                                                     std::make_shared<spdlog::sinks::stdout_color_sink_mt>(), spdlog::thread_pool());
            logger_ptr->set_pattern("%v");

            while (not st.stop_requested())
            {
                if (not self.server_process_ptr_->running())
                {
                    break;
                }
                try
                {
                    asio::read_until(*server_popen, asio::dynamic_buffer(output_buffer), '\n');
                }
                catch (...)
                {
                    logger_ptr->info("exception");
                    break;
                }
                if (output_buffer.empty())
                {
                    continue;
                }
                while (true)
                {
                    auto pos = output_buffer.find('\n');
                    if (pos == std::string::npos)
                    {
                        break;
                    }
                    output_buffer.erase(pos);
                }
                while (true)
                {
                    auto pos = output_buffer.find('\r');
                    if (pos == std::string::npos)
                    {
                        break;
                    }
                    output_buffer.erase(pos);
                }
                if (auto info_opt = self.parser_->parseServerOutput(output_buffer); info_opt.has_value())
                {
                    ServerOutputProcessThread::getInstance().addServerOutputInfo(info_opt.value());
                }
                logger_ptr->info(output_buffer);
                output_buffer.clear();
            }
            self.setServerState(ServerState::SERVER_STATE_STOPPED);
            events::EventBus::getInstancePointer()->postponeEvent<events::ServerStopped>(events::ServerStopped{});
            server_logger->info("Server stopped with return code {}", self.server_process_ptr_->exit_code());
            server_logger->info("Server output processing thread stopping");
        });

        self.server_state_ = ServerState::SERVER_STATE_RUNNING;

        return true;
    }

    auto stop(this auto &&self) -> bool
    {
        if (FWD(self).server_state_ != ServerState::SERVER_STATE_RUNNING)
        {
            return false;
        }
        FWD(self).sendRawInput(FWD(self).parser_->getStopCommand());
        FWD(self).server_process_ptr_->wait();
        FWD(self).server_state_ = ServerState::SERVER_STATE_STOPPED;
        if (FWD(self).output_processing_thread_)
        {
            FWD(self).output_processing_thread_->request_stop();
            if (FWD(self).output_processing_thread_->joinable())
            {
                FWD(self).output_processing_thread_->join();
            }
            FWD(self).output_processing_thread_.reset();
        }
        return true;
    }

    auto kill(this auto &&self) -> bool
    {
        if (FWD(self).server_state_ != ServerState::SERVER_STATE_RUNNING)
        {
            return false;
        }
        FWD(self).server_process_ptr_->terminate();
        FWD(self).server_process_ptr_->wait();
        FWD(self).server_state_ = ServerState::SERVER_STATE_STOPPED;
        if (FWD(self).output_processing_thread_)
        {
            FWD(self).output_processing_thread_->request_stop();
            if (FWD(self).output_processing_thread_->joinable())
            {
                FWD(self).output_processing_thread_->join();
            }
            FWD(self).output_processing_thread_.reset();
        }
        return true;
    }

    auto getServerState(this auto &&self) -> ServerState
    {
        return FWD(self).server_state_;
    }

    auto sendMessage(this auto &&self, std::string const &target, std::string const &info) -> bool
    {
        return FWD(self).sendRawInput(FWD(self).parser_->getSendMessageCommand(target, info));
    }

    auto sendMessage(this auto &&self, std::string const &target, RText const &info) -> bool
    {
        return FWD(self).sendRawInput(FWD(self).parser_->getSendMessageCommand(target, info));
    }

    auto broadcastMessage(this auto &&self, std::string const &info) -> bool
    {
        return FWD(self).sendRawInput(FWD(self).parser_->getBroadcastMessageCommand(info));
    }
    auto broadcastMessage(this auto &&self, RText const &info) -> bool
    {
        return FWD(self).sendRawInput(FWD(self).parser_->getBroadcastMessageCommand(info));
    }

    auto getPath(this auto &&self) -> fs::path
    {
        return FWD(self).server_path_;
    }
};
} // namespace helium::server