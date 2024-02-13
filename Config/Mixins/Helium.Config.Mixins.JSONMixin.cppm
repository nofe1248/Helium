/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <filesystem>

#include <simdjson.h>

#include <nlohmann/json.hpp>

export module Helium.Config.Mixins.JSONMixin;

import Helium.Config.ConfigImpl;

export namespace helium::config::mixins {
    namespace fs = std::filesystem;

    class SimdJsonMixin {
        friend ConfigImpl<SimdJsonMixin>;

    private:
        fs::path config_path_;

        SimdJsonMixin(fs::path const &config_path) : config_path_(config_path) {}
        SimdJsonMixin(fs::path &&config_path) : config_path_(std::move(config_path)) {}

    public:
    };

    class NlohmannJsonMixin {
        friend ConfigImpl<SimdJsonMixin>;

    private:
        fs::path config_path_;

        NlohmannJsonMixin(fs::path const &config_path) : config_path_(config_path) {}
        NlohmannJsonMixin(fs::path &&config_path) : config_path_(std::move(config_path)) {}

    public:
    };

    using DefaultJsonMixin = SimdJsonMixin;
} // namespace helium::config::mixins
