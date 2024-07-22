cmake_minimum_required(VERSION 3.29 FATAL_ERROR)
cmake_policy(VERSION 3.29)

set(LD_LIBRARY_PATH "C:/msys64/ucrt64/bin/")

project(Helium LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
        cpptrace
        GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git
        GIT_TAG v0.6.2 # <HASH or TAG>
)
FetchContent_MakeAvailable(cpptrace)
FetchContent_Declare(
        graaflib
        GIT_REPOSITORY https://github.com/bobluppes/graaf.git
        GIT_TAG main
)
set(SKIP_TESTS ON)
set(SKIP_BENCHMARKS ON)
set(SKIP_EXAMPLES ON)
FetchContent_MakeAvailable(graaflib)
find_package(tomlplusplus CONFIG REQUIRED)
find_package(reflectcpp CONFIG REQUIRED)
set(REFLECTCPP_TOML ON)
find_path(BEXT_SML2_INCLUDE_DIRS "boost/sml2")
find_package(Catch2 3 REQUIRED)
find_package(Boost 1.85.0 REQUIRED COMPONENTS filesystem)
find_package(fmt CONFIG REQUIRED)
find_package(mpark_patterns CONFIG REQUIRED)
find_package(nameof CONFIG REQUIRED)
find_path(semver "neargye/semver.hpp")
find_path(PLF_HIVE_INCLUDE_DIRS "plf_hive.h")
find_package(spdlog CONFIG REQUIRED)
find_package(cxxopts CONFIG REQUIRED)
find_package(Python COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG)
find_package(rapidfuzz CONFIG REQUIRED)
find_package(replxx CONFIG REQUIRED)
find_package(proxy CONFIG REQUIRED)
set(stdexec_DIR "C:/vcpkg/installed/x64-mingw-dynamic/share/stdexec")
find_package(stdexec CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_CXX_EXTENSIONS ON)

add_executable(Helium)

target_sources(
    Helium PRIVATE
    FILE_SET helium_cxx_modules TYPE CXX_MODULES FILES
        Base/HeliumObject/Helium.Base.HeliumObject.cppm
        Base/HeliumSharedObject/Helium.Base.HeliumSharedObject.cppm
        Base/Test/Helium.Base.Test.cppm
        Base/Version/Helium.Base.Version.cppm
        Base/Helium.Base.cppm
        CLI/Helium.CLI.cppm
        CLI/CLIMainLoop/Helium.CLI.MainCLILoop.cppm
        Commands/BindingHelper/Helium.Commands.BindingHelper.cppm
        Commands/CommandArgument/Helium.Commands.CommandArgument.CommandArgumentBase.cppm
        Commands/CommandArgument/Helium.Commands.CommandArgument.CommandArgumentBoolean.cppm
        Commands/CommandArgument/Helium.Commands.CommandArgument.CommandArgumentFloatingPoint.cppm
        Commands/CommandArgument/Helium.Commands.CommandArgument.CommandArgumentInteger.cppm
        Commands/CommandArgument/Helium.Commands.CommandArgument.CommandArgumentQuotedString.cppm
        Commands/CommandArgument/Helium.Commands.CommandArgument.CommandArgumentString.cppm
        Commands/CommandArgument/Helium.Commands.CommandArgument.cppm
        Commands/CommandRoot/Helium.Commands.CommandRoot.cppm
        Commands/CommandBase/Helium.Commands.CommandBase.cppm
        Commands/CommandContext/Helium.Commands.CommandContext.cppm
        Commands/CommandDispatcher/Helium.Commands.CommandDispatcher.cppm
        Commands/CommandLiteral/Helium.Commands.CommandLiteral.CommandLiteralBase.cppm
        Commands/CommandLiteral/Helium.Commands.CommandLiteral.CommandStringLiteral.cppm
        Commands/CommandLiteral/Helium.Commands.CommandLiteral.cppm
        Commands/CommandSource/Helium.Commands.CommandSource.cppm
        Commands/Concepts/Helium.Commands.Concepts.cppm
        Commands/Lexer/Helium.Commands.Lexer.cppm
        Commands/Test/Helium.Commands.Test.cppm
        Commands/Helium.Commands.cppm
        Config/Helium.Config.cppm
        Events/BindingHelper/Helium.Events.BindingHelper.cppm
        Events/Concepts/Helium.Events.Concepts.cppm
        Events/EventBus/Helium.Events.EventBus.cppm
        Events/EventEmitter/Helium.Events.EventEmitter.cppm
        Events/EventListener/Helium.Events.EventListener.cppm
        Events/EventStream/Helium.Events.EventStream.cppm
        Events/Helium/Helium.Events.Helium.cppm
        Events/MainEventLoop/Helium.Events.MainEventLoop.cppm
        Events/Test/Helium.Events.Test.cppm
        Events/Helium.Events.cppm
        Exceptions/TerminateHandler/Helium.Exceptions.TerminateHandler.cppm
        Exceptions/Helium.Exceptions.cppm
        Logger/LoggerImpl/Helium.Logger.LoggerImpl.cppm
        Logger/SharedLogger/Helium.Logger.SharedLogger.cppm
        Logger/Test/Helium.Logger.Test.cppm
        Logger/Helium.Logger.cppm
        Main/Helium.Main.cppm
        Parser/Helium.Parser.cppm
        Plugins/Bindings/Helium.Plugins.Bindings.cppm
        Plugins/Bindings/Helium.Plugins.Bindings.Python.cppm
        Plugins/Helium.Plugins.cppm
        Plugins/PluginInstance/Helium.Plugins.PluginInstance.cppm
        Plugins/PluginManager/Helium.Plugins.PluginManager.cppm
        Plugins/PluginMetadata/Helium.Plugins.PluginMetadata.cppm
        Plugins/Test/Helium.Plugins.Test.cppm
        Server/Helium.Server.cppm
        Server/ServerInstance/Helium.Server.ServerInstance.cppm
        Utils/OverloadSet/Helium.Utils.OverloadSet.cppm
        Utils/RunLoopExecutor/Helium.Utils.RunLoopExecutor.cppm
        Utils/Test/Helium.Utils.Test.cppm
        Utils/Helium.Utils.cppm
)
target_link_libraries(Helium PRIVATE ws2_32 wsock32 bcrypt pthread stdc++)

target_include_directories(Helium PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(Helium PRIVATE Boost::filesystem)

target_include_directories(Helium PRIVATE semver)
target_include_directories(Helium PRIVATE ${PLF_HIVE_INCLUDE_DIRS})
target_include_directories(Helium PRIVATE ${BEXT_SML2_INCLUDE_DIRS})

target_link_libraries(Helium PRIVATE replxx::replxx)
target_link_libraries(Helium PRIVATE msft_proxy)
target_link_libraries(Helium PRIVATE rapidfuzz::rapidfuzz)
target_link_libraries(Helium PRIVATE pybind11::embed)
target_link_libraries(Helium PRIVATE cxxopts::cxxopts)
target_link_libraries(Helium PRIVATE spdlog::spdlog)
target_link_libraries(Helium PRIVATE Catch2::Catch2)
target_link_libraries(Helium PRIVATE nameof::nameof)
target_link_libraries(Helium PRIVATE mpark_patterns)
target_link_libraries(Helium PRIVATE fmt::fmt)
target_link_libraries(Helium PRIVATE reflectcpp::reflectcpp)
target_link_libraries(Helium PRIVATE tomlplusplus::tomlplusplus)
target_link_libraries(Helium PRIVATE Graaf::Graaf)
target_link_libraries(Helium PRIVATE cpptrace::cpptrace)
target_link_libraries(Helium PRIVATE STDEXEC::stdexec)