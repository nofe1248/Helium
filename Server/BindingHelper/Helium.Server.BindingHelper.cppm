/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <pybind11/chrono.h>
#include <pybind11/embed.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>

export module Helium.Server.BindingHelper;

import Helium.Server.ServerInstance;
import Helium.Server.ServerOutputParser;

namespace py = pybind11;

export namespace helium::server::binding
{
class ServerInstanceBindingHelper
{
};
} // namespace helium::server::binding