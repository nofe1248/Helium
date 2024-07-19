/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <exception>
#include <format>
#include <iomanip>
#include <iostream>

#include <unistd.h>

#include <cpptrace/cpptrace.hpp>

#include <spdlog/spdlog.h>

#ifdef _WIN32
#include <windows.h>
#endif

export module Helium.Exceptions.TerminateHandler;

namespace helium::exceptions
{
auto printTerminateTrace() -> void
{
    cpptrace::generate_trace(3).print_with_snippets();
}
auto terminateHandler() -> void
{
    spdlog::shutdown();
    std::cerr << "Helium crashed, please send the crash stacktrace and log to the Helium DevTeam.\n";
    try
    {
        auto ptr = std::current_exception();
        if (ptr == nullptr)
        {
            std::cerr << "terminate called without an active exception\n";
            printTerminateTrace();
        }
        else
        {
            std::rethrow_exception(ptr);
        }
    }
    catch (cpptrace::exception &e)
    {
        std::cerr << std::format("Terminate called after throwing an instance of {}: {}\n", cpptrace::demangle(typeid(e).name()), e.message());
        e.trace().print(std::cerr);
    }
    catch (std::exception &e)
    {
        std::cerr << std::format("Terminate called after throwing an instance of {}: {}\n", cpptrace::demangle(typeid(e).name()), e.what());
        printTerminateTrace();
    }
    catch (...)
    {
        std::cerr << std::format("Terminate called after throwing an instance of an unrecognized exception.\n");
        printTerminateTrace();
    }
    std::flush(std::cerr);
    abort();
}
#ifdef _WIN32
auto WINAPI topLevelExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo) -> LONG
{
    sleep(10);
    std::cerr << "\n";
    std::cerr << std::hex << pExceptionInfo->ExceptionRecord->ExceptionCode << "(" << pExceptionInfo->ExceptionRecord->ExceptionInformation << ")"
              << " at " << pExceptionInfo->ExceptionRecord->ExceptionAddress << std::endl;
    terminateHandler();
}
#endif
struct TerminationHandlerHelper
{
    TerminationHandlerHelper()
    {
        std::set_terminate(terminateHandler);
#ifdef _WIN32
        SetUnhandledExceptionFilter(topLevelExceptionHandler);
#endif
    }
};
TerminationHandlerHelper helper;
} // namespace helium::exceptions