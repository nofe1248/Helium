/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <any>
#include <exception>
#include <functional>

#include <stdexec/execution.hpp>

#include <pybind11/pybind11.h>

export module Helium.Utils.RunLoopExecutor;

import Helium.Base.HeliumObject;
import Helium.Logger;

namespace stdex = stdexec;
namespace py = pybind11;

namespace helium::utils
{
auto executor_logger = logger::SharedLogger::getSharedLogger("Utils", "RunLoopExecutor");
}

export namespace helium::utils
{
class RunLoopExecutor final : public base::HeliumObject
{
private:
    stdex::run_loop loop_;

public:
    RunLoopExecutor() : loop_{}
    {
    }
    RunLoopExecutor(const RunLoopExecutor &) = delete;
    RunLoopExecutor &operator=(const RunLoopExecutor &) = delete;
    RunLoopExecutor(RunLoopExecutor &&) noexcept = delete;
    RunLoopExecutor &operator=(RunLoopExecutor &&) noexcept = delete;

    ~RunLoopExecutor()
    {
    }

    static auto getInstance() noexcept -> RunLoopExecutor &
    {
        static RunLoopExecutor run_loop_executor;
        return run_loop_executor;
    }

    auto run(this auto &&self) noexcept -> void
    {
        executor_logger->info("Executor running");
        std::forward<decltype(self)>(self).loop_.run();
    }

    auto finish(this auto &&self) noexcept -> void
    {
        executor_logger->info("Executor finishing");
        std::forward<decltype(self)>(self).loop_.finish();
    }

    [[nodiscard]] auto get_scheduler(this auto &&self) noexcept -> stdex::scheduler auto
    {
        return std::forward<decltype(self)>(self).loop_.get_scheduler();
    }

    // use std::function instead of a templated function to sidestep a clang bug related to lambda name mangling in modules
    template <typename RetType>
    [[nodiscard]] auto execute(this auto &&self, std::function<RetType()> const &func) noexcept -> RetType
    {
        auto [result] = stdex::sync_wait(stdex::then(stdex::schedule(std::forward<decltype(self)>(self).loop_.get_scheduler()), [&func]() {
                            try
                            {
                                return func();
                            }
                            catch (py::error_already_set const &e)
                            {
                                executor_logger->error("Exception during execution: {}", e.what());
                            }
                            catch (std::exception const &e)
                            {
                                executor_logger->error("Exception during execution: {}", e.what());
                            }
                            catch (...)
                            {
                                executor_logger->error("Unknown exception during execution");
                            }
                        })).value();
        return result;
    }

    auto execute(this auto &&self, std::function<void()> const &func) noexcept -> void
    {
        stdex::sync_wait(stdex::then(stdex::schedule(std::forward<decltype(self)>(self).loop_.get_scheduler()), [&func]() {
            try
            {
                func();
            }
            catch (py::error_already_set const &e)
            {
                executor_logger->error("Exception during execution: {}", e.what());
            }
            catch (std::exception const &e)
            {
                executor_logger->error("Exception during execution: {}", e.what());
            }
            catch (...)
            {
                executor_logger->error("Unknown exception during execution");
            }
        }));
    }
};
} // namespace helium::utils