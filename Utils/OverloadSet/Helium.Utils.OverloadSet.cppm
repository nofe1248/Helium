/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

export module Helium.Utils.OverloadSet;

export namespace helium::utils {	
	template <typename ... Ts>
	struct OverloadSet : public Ts... {
		using Ts::operator()...;

		consteval auto operator()(auto ... dummy) -> void { 
			//We need this until MSVC support the C++26 Pack Indexing
			//If a future version of MSVC officially support the feature,
			//the boost::mp11::mp_nth_element should be replaced by pack...[I]
			static_assert(false, "Unsupported type.");
		}
	};
}