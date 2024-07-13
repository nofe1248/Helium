/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

export module Helium.Events.Helium;

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
};
struct ServerOutput
{
};
struct ServerInput
{
};
struct GeneralInput
{
};
struct PythonEvent
{
};
} // namespace helium::events