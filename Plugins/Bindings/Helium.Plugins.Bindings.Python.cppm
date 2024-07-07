/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <pybind11/embed.h>

export module Helium.Plugins.Bindings.Python;

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(helium, m)
{
}