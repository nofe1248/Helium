/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */
module;

#include <semver.hpp>

export module Helium.Base.Version;

export namespace helium::base {
	constexpr semver::version helium_version{0, 0, 1, semver::prerelease::alpha, 0};
}