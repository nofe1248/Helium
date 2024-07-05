/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <format>
#include <string>
#include <utility>

#include <nameof.hpp>

#define FWD(x) ::std::forward<decltype(x)>(x)

export module Helium.Base.HeliumObject;

export namespace helium::base {
    class HeliumObject {
    public:
        auto toString(this auto &&self) -> std::string {
            return std::format(
                "{}(size: {}, align: {}) at 0x{:X}",
                nameof::nameof_type<decltype(self)>(),
                sizeof(decltype(self)),
                alignof(decltype(self)),
                reinterpret_cast<ptrdiff_t>(std::addressof(self))
            );
        }
    };
}