/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <string>

#include <pybind11/embed.h>

export module Helium.Events.Helium;

namespace py = pybind11;

export namespace helium::events
{
struct HeliumStarting
{
};
struct HeliumStarted
{
    HeliumStarted() {}
};
struct HeliumStopping
{
};
struct PluginLoaded
{
};
struct PluginUnloaded
{
};
struct PluginReloaded
{
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
struct ServerPaused
{
};
struct ServerResumed
{
};
struct ConsoleInput
{
    std::string input;
};
struct ServerOutputRaw
{
    std::string output;
};
struct PlayerInputRaw
{
    std::string input;
};
struct ServerOutput
{
};
struct PlayerInput
{
};
struct PythonEvent
{
    std::string event_id;
    py::object event_arg;
};
} // namespace helium::events