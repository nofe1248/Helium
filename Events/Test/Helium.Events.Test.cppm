/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <print>

#define BOOST_UT_DISABLE_MODULE
#include <boost/ut.hpp>

export module Helium.Events.Test;

import Helium.Events.EventEmitter;
import Helium.Events.Policy;

export namespace helium::events::test {
    using namespace boost::ut;

    template<typename Base>
    struct A : public Base {
        using super = Base;
        using super::super;
        int a() { return 0; };
    };
    template<typename Base>
    struct B : public Base {
        using super = Base;
        using super::super;
        double b() { return 1.0f; };
    };
    template<typename Base>
    struct C : public Base {
        using super = Base;
        using super::super;
        bool c() { return true; };
    };
    struct MyPolicy {
        using Mixins = EventEmitterMixinList<A, B, C>;
    };

    auto testModule() -> void {
        boost::ut::test("Helium.Events Module Test #1") = [] {
            EventEmitter<MyPolicy> emitter;
            emitter.a();
            emitter.b();
            emitter.c();
        };
    }
} // namespace helium::events::test
