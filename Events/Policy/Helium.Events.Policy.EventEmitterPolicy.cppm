/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

export module Helium.Events.Policy.EventEmitterPolicy;

import Helium.Events.Concepts;

import Helium.Events.Policy.PolicyHelper;

export namespace helium::events {
    template<template<typename> class... Mixins_>
    using EventEmitterMixinList = MixinList<Mixins_...>;
} // namespace helium::events