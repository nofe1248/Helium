cmake_minimum_required(VERSION 3.28)

project(Helium CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

cmake_policy(VERSION 3.28)

add_executable(Helium)

target_sources(
    Helium PRIVATE
    FILE_SET helium_cxx_modules TYPE CXX_MODULES FILES
        Base/HeliumObject/Helium.Base.HeliumObject.cppm
        Base/Test/Helium.Base.Test.cppm
        Base/Version/Helium.Base.Version.cppm
        Base/Helium.Base.cppm
        Commands/CommandArgument/Helium.Commands.CommandArgument.CommandArgumentBoolean.cppm
        Commands/CommandArgument/Helium.Commands.CommandArgument.CommandArgumentFloatingPoint.cppm
        Commands/CommandArgument/Helium.Commands.CommandArgument.CommandArgumentGreedyString.cppm
        Commands/CommandArgument/Helium.Commands.CommandArgument.CommandArgumentInteger.cppm
        Commands/CommandArgument/Helium.Commands.CommandArgument.CommandArgumentQuotedString.cppm
        Commands/CommandArgument/Helium.Commands.CommandArgument.CommandArgumentString.cppm
        Commands/CommandArgument/Helium.Commands.CommandArgument.cppm
        Commands/CommandBase/Helium.Commands.CommandBase.cppm
        Commands/CommandContext/Helium.Commands.CommandContext.cppm
        Commands/CommandDispatcher/Helium.Commands.CommandDispatcher.cppm
        Commands/CommandLiteral/Helium.Commands.CommandLiteral.CommandStringLiteral.cppm
        Commands/CommandLiteral/Helium.Commands.CommandLiteral.cppm
        Commands/Concepts/Helium.Commands.Concepts.cppm
        Commands/Lexer/Helium.Commands.Lexer.cppm
        Commands/Test/Helium.Commands.Test.cppm
        Commands/Helium.Commands.cppm
        Config/ConfigManager/Helium.Config.ConfigManager.cppm
        Config/Test/Helium.Config.Test.cppm
        Config/Helium.Config.cppm
        Events/Concepts/Helium.Events.Concepts.cppm
        Events/EventEmitter/Helium.Events.EventEmitter.cppm
        Events/EventEmitterPolicy/Helium.Events.EventEmitterPolicy.cppm
        Events/EventListener/Helium.Events.EventListener.cppm
        Events/EventManager/Helium.Events.EventManager.cppm
        Events/Helium/Helium.Events.Helium.cppm
        Events/ListenerStream/Helium.Events.ListenerStream.cppm
        Events/Test/Helium.Events.Test.cppm
        Events/Helium.Events.cppm
        Exceptions/TerminateHandler/Helium.Exceptions.TerminateHandler.cppm
        Exceptions/Helium.Exceptions.cppm
        Logger/LoggerImpl/Helium.Logger.LoggerImpl.cppm
        Logger/Test/Helium.Logger.Test.cppm
        Logger/Helium.Logger.cppm
        Main/Helium.Main.cppm
        Modules/Helium.Modules.cppm
        Modules/Module/Helium.Modules.Module.cppm
        Modules/ModuleManager/Helium.Modules.ModuleManager.cppm
        Modules/ModuleMetadata/Helium.Modules.ModuleMetadata.cppm
        Modules/Test/Helium.Modules.Test.cppm
        Utils/OverloadSet/Helium.Utils.OverloadSet.cppm
        Utils/Test/Helium.Utils.Test.cppm
        Utils/Helium.Utils.cppm
)

find_package(Backward CONFIG REQUIRED)
target_link_libraries(Helium PRIVATE Backward::Backward)

find_path(BEXT_SML2_INCLUDE_DIRS "boost/sml2")
target_include_directories(Helium PRIVATE ${BEXT_SML2_INCLUDE_DIRS})

find_package(ut CONFIG REQUIRED)
target_link_libraries(Helium PRIVATE Boost::ut)

find_package(Boost REQUIRED context filesystem program_options regex system thread chrono atomic)
target_link_libraries(Helium PRIVATE Boost::boost Boost::context Boost::filesystem Boost::program_options Boost::regex Boost::system Boost::thread Boost::chrono Boost::atomic)

#find_package(concurrencpp CONFIG REQUIRED)
#target_link_libraries(Helium PRIVATE concurrencpp::concurrencpp)

#find_package(flux CONFIG REQUIRED)
#target_link_libraries(Helium PRIVATE flux::flux)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(Helium PRIVATE fmt::fmt)

#find_package(FunctionalPlus CONFIG REQUIRED)
#target_link_libraries(Helium PRIVATE FunctionalPlus::fplus)

find_package(libdwarf CONFIG REQUIRED)
target_link_libraries(Helium PRIVATE libdwarf::dwarf)

find_package(mpark_patterns CONFIG REQUIRED)
target_link_libraries(Helium PRIVATE mpark_patterns)

find_package(Microsoft.GSL CONFIG REQUIRED)
target_link_libraries(Helium PRIVATE Microsoft.GSL::GSL)

find_package(nameof CONFIG REQUIRED)
target_link_libraries(Helium PRIVATE nameof::nameof)

find_path(NEARGYE_SEMVER_INCLUDE_DIRS "neargye/semver.hpp")
target_include_directories(Helium PRIVATE ${NEARGYE_SEMVER_INCLUDE_DIRS})

find_path(PLF_COLONY_INCLUDE_DIRS "plf_colony.h")
target_include_directories(Helium PRIVATE ${PLF_COLONY_INCLUDE_DIRS})

find_package(proxy CONFIG REQUIRED)
target_link_libraries(Helium PRIVATE msft_proxy)

find_path(SCOPE_GUARD_INCLUDE_DIRS "scope_guard.hpp")
target_include_directories(Helium PRIVATE ${SCOPE_GUARD_INCLUDE_DIRS})

find_package(simdjson CONFIG REQUIRED)
target_link_libraries(Helium PRIVATE simdjson::simdjson)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(Helium PRIVATE spdlog::spdlog)

find_package(stdexec CONFIG REQUIRED)
target_link_libraries(Helium PRIVATE STDEXEC::stdexec)