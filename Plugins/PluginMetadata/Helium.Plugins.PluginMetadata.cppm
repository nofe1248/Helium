/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <string>
#include <optional>
#include <vector>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(_WIN64)
	#define BOOST_UUID_RANDOM_PROVIDER_FORCE_WINCRYPT
#endif

#include <boost/container_hash/hash.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <semver.hpp>

export module Helium.Plugins.PluginMetadata;

export namespace helium::plugins {
	struct PluginMetadata {
		PluginMetadata() {
			this->uuid = boost::uuids::random_generator()();
		}

		struct ModuleDependency {
			std::string									name;
			semver::version								version;
		};
		std::string										entry;
		std::string										name;
		semver::version									version;
		boost::uuids::uuid								uuid;
		std::optional<std::string>						description;
		std::optional<std::string>						author;
		std::optional<std::vector<ModuleDependency>>	dependencies;
		std::optional<std::vector<ModuleDependency>>	optional_dependencies;
		std::optional<std::vector<ModuleDependency>>	conflicts;
		std::optional<std::vector<ModuleDependency>>	load_before;
	};

	struct PluginMetadataHash {
		auto operator()(helium::plugins::PluginMetadata const& metadata) const noexcept -> std::size_t {
			return boost::hash<boost::uuids::uuid>()(metadata.uuid);
		}
	};
}