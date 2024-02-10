/*
* Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <string>

export module Helium.Commands.Parser;

import Helium.Base;
import Helium.Commands.Concepts;

export namespace helium::commands {
    template <concepts::IsString StrType_>
    class CommandsParser : public HeliumObject {
    public:
        using StringType = StrType_;
        using CharType = typename StringType::value_type;
        using StringViewType = std::basic_string_view<CharType>;
    };
}