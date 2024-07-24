/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <any>
#include <functional>

#include <stdexec/execution.hpp>

export module Helium.Utils.RunLoopExecutor;

import Helium.Base.HeliumObject;
import Helium.Logger;

namespace stdex = stdexec;

namespace helium::utils
{
auto executor_logger = logger::SharedLogger::getSharedLogger("Utils", "RunLoopExecutor");
}

export namespace helium::utils
{
struct NeedReturn
{
} need_return;
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
    [[nodiscard]] auto execute(this auto &&self, std::function<std::any()> &&func, NeedReturn need_return) noexcept -> std::any
    {
        return stdex::sync_wait(
                   stdex::then(stdex::schedule(std::forward<decltype(self)>(self).loop_.get_scheduler()), std::forward<decltype(func)>(func)))
            .value();
    }

    auto execute(this auto &&self, std::function<void()> &&func) noexcept -> void
    {
        stdex::sync_wait(stdex::then(stdex::schedule(std::forward<decltype(self)>(self).loop_.get_scheduler()), std::forward<decltype(func)>(func)));
    }
};
} // namespace helium::utils