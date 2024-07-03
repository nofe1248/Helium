/*
* Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

export module Helium.Commands.CommandContext;

import Helium.Base;
import Helium.Commands.CommandSource;

export namespace helium::commands {
    class CommandContext : public base::HeliumObject {
    private:
        CommandSource source_;

    public:
        CommandContext(CommandSource &&source) : source_(source) {}
        CommandContext(CommandSource const& source) : source_(source) {}
    };
}