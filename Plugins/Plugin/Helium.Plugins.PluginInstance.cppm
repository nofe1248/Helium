/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <memory>

#include <boost/dll.hpp>
#include <boost/dll/import_class.hpp>
#include <boost/dll/import_mangled.hpp>
#include <boost/dll/smart_library.hpp>

export module Helium.Plugins.PluginInstance;

import Helium.Base;

export namespace helium::plugins
{
class PluginInstance : public base::HeliumObject
{
};
} // namespace helium::modules