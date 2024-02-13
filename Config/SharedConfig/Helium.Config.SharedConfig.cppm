/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

export module Helium.Config.SharedConfig;

import Helium.Base.HeliumSharedObject;
import Helium.Config.ConfigImpl;

export namespace helium::config {
    template<typename Mixin_>
    class SharedConfig : public base::HeliumSharedObjectBase<SharedConfig<Mixin_>, ConfigImpl<Mixin_>> {
    public:
        using Mixin = Mixin_;
        using base::HeliumSharedObjectBase<SharedConfig<Mixin_>, ConfigImpl<Mixin_>>::HeliumSharedObjectBase;
    };
} // namespace helium::config
