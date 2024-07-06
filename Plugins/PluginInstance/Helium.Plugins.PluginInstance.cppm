/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <memory>

#include <pybind11/embed.h>

export module Helium.Plugins.PluginInstance;

import Helium.Base;

namespace py = pybind11;

export namespace helium::plugins
{
class PluginInstance : public base::HeliumObject
{
};
} // namespace helium::modules