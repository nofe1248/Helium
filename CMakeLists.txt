cmake_minimum_required(VERSION 3.29 FATAL_ERROR)
cmake_policy(VERSION 3.29)

project(Helium LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

add_compile_definitions(BOOST_UT_DISABLE_MODULE)

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
        CLI/CLIMainLoop/Helium.CLI.CLIMainLoop.cppm
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
        Events/Concepts/Helium.Events.Concepts.cppm
        Events/EventBus/Helium.Events.EventBus.cppm
        Events/EventEmitter/Helium.Events.EventEmitter.cppm
        Events/EventListener/Helium.Events.EventListener.cppm
        Events/EventManager/Helium.Events.EventManager.cppm
        Events/Helium/Helium.Events.Helium.cppm
        Events/ListenerStream/Helium.Events.ListenerStream.cppm
        Events/Policy/Helium.Events.Policy.cppm
        Events/Policy/Helium.Events.Policy.EventEmitterPolicy.cppm
        Events/Policy/Helium.Events.Policy.EventListenerPolicy.cppm
        Events/Policy/Helium.Events.Policy.PolicyHelper.cppm
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
        Plugins/Helium.Plugins.cppm
        Plugins/Plugin/Helium.Plugins.PluginInstance.cppm
        Plugins/PluginManager/Helium.Plugins.PluginManager.cppm
        Plugins/PluginMetadata/Helium.Plugins.PluginMetadata.cppm
        Plugins/Test/Helium.Plugins.Test.cppm
        Server/Helium.Server.cppm
        Utils/OverloadSet/Helium.Utils.OverloadSet.cppm
        Utils/Test/Helium.Utils.Test.cppm
        Utils/Helium.Utils.cppm
)

include(FetchContent)
FetchContent_Declare(
        cpptrace
        GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git
        GIT_TAG        v0.6.2 # <HASH or TAG>
)
FetchContent_MakeAvailable(cpptrace)
target_link_libraries(Helium PRIVATE cpptrace::cpptrace)

find_path(BEXT_SML2_INCLUDE_DIRS "boost/sml2")
target_include_directories(Helium PRIVATE ${BEXT_SML2_INCLUDE_DIRS})

find_package(Catch2 3 REQUIRED)
target_link_libraries(Helium PRIVATE Catch2::Catch2)

find_package(Boost REQUIRED context filesystem program_options regex system thread chrono atomic)
target_link_libraries(Helium PRIVATE Boost::boost Boost::context Boost::filesystem Boost::program_options Boost::regex Boost::system Boost::thread Boost::chrono Boost::atomic)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(Helium PRIVATE fmt::fmt)

find_package(mpark_patterns CONFIG REQUIRED)
target_link_libraries(Helium PRIVATE mpark_patterns)

find_package(Microsoft.GSL CONFIG REQUIRED)
target_link_libraries(Helium PRIVATE Microsoft.GSL::GSL)

find_package(nameof CONFIG REQUIRED)
target_link_libraries(Helium PRIVATE nameof::nameof)

find_path(semver "neargye/semver.hpp")
target_include_directories(Helium PRIVATE semver)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(Helium PRIVATE nlohmann_json::nlohmann_json)

find_path(PLF_HIVE_INCLUDE_DIRS "plf_hive.h")
target_include_directories(Helium PRIVATE ${PLF_HIVE_INCLUDE_DIRS})

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(Helium PRIVATE spdlog::spdlog)

find_package(stdexec CONFIG REQUIRED)
target_link_libraries(Helium PRIVATE STDEXEC::stdexec)

find_package(cxxopts CONFIG REQUIRED)
target_link_libraries(Helium PRIVATE cxxopts::cxxopts)

find_package(Python COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG)
target_link_libraries(Helium PRIVATE pybind11::embed)

find_package(rapidfuzz CONFIG REQUIRED)
target_link_libraries(Helium PRIVATE rapidfuzz::rapidfuzz)

#list(APPEND CMAKE_MODULE_PATH "C:\\Program Files (x86)\\replxx\\share\\cmake\\replxx")
find_package(replxx CONFIG REQUIRED)
target_link_libraries(Helium PRIVATE replxx::replxx)