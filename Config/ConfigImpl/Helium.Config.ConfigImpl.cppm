/*
* Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

export module Helium.Config.ConfigImpl;

import Helium.Base;

export namespace helium::config {
    template<typename Mixin_>
    class ConfigImpl : public base::HeliumObject, public Mixin_ {
    public:
        using Mixin = Mixin_;
    };
} // namespace helium::config
