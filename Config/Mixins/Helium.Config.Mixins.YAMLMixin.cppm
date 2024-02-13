/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <filesystem>
#include <utility>

#include <yaml-cpp/yaml.h>

#include <ryml/ryml.hpp>

export module Helium.Config.Mixins.YAMLMixin;

import Helium.Config.ConfigImpl;

export namespace helium::config::mixins {
    namespace fs = std::filesystem;
    class YamlCppMixin {
        friend ConfigImpl<YamlCppMixin>;

    private:
        fs::path config_path_;
        YAML::Node config_;

        YamlCppMixin(fs::path const &config_path) : config_path_(config_path) {

        }
        YamlCppMixin(fs::path &&config_path) noexcept : config_path_(std::move(config_path)) {

        }

    public:
        auto loadFile() -> void {
            this->config_ = YAML::LoadFile(this->config_path_.string());
        }

        auto getPath() const noexcept -> fs::path { return this->config_path_; }
    };

    class RymlMixin {
        friend ConfigImpl<RymlMixin>;

    private:
        fs::path config_path_;

        RymlMixin(fs::path const &config_path) : config_path_(config_path) {}
        RymlMixin(fs::path &&config_path) : config_path_(std::move(config_path)) {}

    public:
    };

    using DefaultYamlMixin = RymlMixin;
} // namespace helium::config::mixins
