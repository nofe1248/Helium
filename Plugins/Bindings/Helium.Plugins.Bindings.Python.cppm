/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <memory>
#include <string>

#include <pybind11/embed.h>
#include <spdlog/logger.h>

export module Helium.Plugins.Bindings.Python;

import Helium.Commands;
import Helium.Config;
import Helium.Logger;

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(helium, m)
{
    using namespace helium;
    // clang-format off
    py::class_<logger::LoggerImpl, std::shared_ptr<logger::LoggerImpl>>(m, "Logger")
        .def(py::init([](std::string const &name, std::string const &thread) {
            return std::make_shared<logger::LoggerImpl>(name, thread);
        }))
        .def("trace", &logger::LoggerImpl::trace_string)
        .def("debug", &logger::LoggerImpl::debug_string)
        .def("info", &logger::LoggerImpl::info_string)
        .def("warn", &logger::LoggerImpl::warn_string)
        .def("error", &logger::LoggerImpl::error_string)
        .def("critical", &logger::LoggerImpl::critical_string)
        .def("flush", &logger::LoggerImpl::flush)
        .def("enable_debug_log", &logger::LoggerImpl::enableDebugLog)
        .def("disable_debug_log", &logger::LoggerImpl::disableDebugLog);

    // clang-format on
}