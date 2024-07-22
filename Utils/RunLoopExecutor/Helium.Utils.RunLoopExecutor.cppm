/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

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

    auto get_scheduler(this auto &&self) noexcept -> stdex::scheduler auto
    {
        return std::forward<decltype(self)>(self).loop_.get_scheduler();
    }

    template <typename FuncT, typename... Args>
    auto add_task(this auto &&self, FuncT &&task, Args &&...args) noexcept -> stdex::sender auto
    {
        return stdex::on(std::forward<decltype(self)>(self).loop_.get_scheduler(),
                         stdex::just(std::forward<Args>(args)...) | stdex::then(std::forward<FuncT>(task)));
    }
};
RunLoopExecutor run_loop_executor;
} // namespace helium::utils