/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <string>

#include <pybind11/embed.h>

export module Helium.Events.Helium;

import Helium.Server.ServerOutputParser.ServerOutputInfo;

namespace py = pybind11;

export namespace helium::events
{
struct HeliumStarting
{
};
struct HeliumStarted
{
};
struct HeliumStopping
{
};
struct PluginLoaded
{
    std::string id;
};
struct PluginUnloaded
{
    std::string id;
};
struct PluginReloaded
{
    std::string id;
};
struct ServerStarting
{
};
struct ServerStarted
{
};
struct ServerStopping
{
};
struct ServerStopped
{
};
struct ConsoleInput
{
    std::string input;
};
struct ServerOutput
{
    server::ServerOutputInfo info{};
};
struct PlayerMessage
{
    server::PlayerMessage info{};
};
struct ServerMessage
{
    server::ServerMessage info{};
};
struct PlayerJoined
{
    server::PlayerJoined info{};
};
struct PlayerLeft
{
    server::PlayerLeft info{};
};
struct ServerAddress
{
    server::ServerAddress info{};
};
struct ServerVersion
{
    server::ServerVersion info{};
};
struct RCONStarted
{
};
struct PythonEvent
{
    std::string event_id;
    py::object event_arg;
};
} // namespace helium::events