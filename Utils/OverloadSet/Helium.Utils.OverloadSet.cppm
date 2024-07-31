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
	};

    template <typename ... Ts>
    OverloadSet(Ts...) -> OverloadSet<Ts...>;
}