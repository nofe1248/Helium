/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.ixx for full license info.
 */

module;

#include <string>

export module Helium.Logger.SharedLogger;

import Helium.Base.HeliumSharedObject;
import Helium.Logger.LoggerImpl;

export namespace helium::logger {
    class SharedLogger : public base::HeliumSharedObjectBase<SharedLogger, LoggerImpl> {
    public:
        using HeliumSharedObjectBase::HeliumSharedObjectBase;

        SharedLogger(std::string_view const name, std::string_view const thread) :
            HeliumSharedObjectBase(HeliumSharedObjectBase::cloneFrom(LoggerImpl(name, thread))) {}
    };
} // namespace helium::logger
