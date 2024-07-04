module;

#include <exception>
#include <format>
#include <iostream>

#include <cpptrace/cpptrace.hpp>

#ifdef _WIN32
#include <windows.h>
#endif

export module Helium.Exceptions.TerminateHandler;

namespace helium::exceptions
{
auto printTerminateTrace() -> void
{
    cpptrace::generate_trace().print_with_snippets();
}
auto terminateHandler() -> void
{
    std::cerr << "\n";
    std::cerr << "Helium crashed due to unexpected signal\n";
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