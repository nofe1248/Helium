/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <string>

#include <semver.hpp>

export module Helium.Base.Version;

export namespace helium::base
{
constexpr semver::version helium_version{0, 2, 11, semver::prerelease::alpha, 1};

auto getHeliumVersionString() -> std::string
{
    return helium_version.to_string();
}
} // namespace helium::base