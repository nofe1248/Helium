/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <string>
#include <tuple>

#include <proxy/proxy.h>

export module Helium.Server.ServerOutputParser;

export import Helium.Server.ServerOutputParser.Arclight;
export import Helium.Server.ServerOutputParser.Beta18;
export import Helium.Server.ServerOutputParser.Bukkit;
export import Helium.Server.ServerOutputParser.BukkitLegacy;
export import Helium.Server.ServerOutputParser.Bungeecord;
export import Helium.Server.ServerOutputParser.CatServer;
export import Helium.Server.ServerOutputParser.Forge;
export import Helium.Server.ServerOutputParser.PythonCustomParserWrapper;
export import Helium.Server.ServerOutputParser.ServerOutputInfo;
export import Helium.Server.ServerOutputParser.Utils;
export import Helium.Server.ServerOutputParser.Vanilla;
export import Helium.Server.ServerOutputParser.Velocity;
export import Helium.Server.ServerOutputParser.Waterfall;

import Helium.Utils.RText;

namespace helium::server::proxy
{
PRO_DEF_MEM_DISPATCH(MemberGetParserName, getParserName);
PRO_DEF_MEM_DISPATCH(MemberGetSendMessageCommand, getSendMessageCommand);
PRO_DEF_MEM_DISPATCH(MemberGetBroadcastMessageCommand, getBroadcastMessageCommand);
PRO_DEF_MEM_DISPATCH(MemberGetStopCommand, getStopCommand);
PRO_DEF_MEM_DISPATCH(MemberPreprocessServerOutput, preprocessServerOutput);
PRO_DEF_MEM_DISPATCH(MemberParseServerOutput, parseServerOutput);
PRO_DEF_MEM_DISPATCH(MemberParsePlayerJoined, parsePlayerJoined);
PRO_DEF_MEM_DISPATCH(MemberParsePlayerLeft, parsePlayerLeft);
PRO_DEF_MEM_DISPATCH(MemberParseServerVersion, parseServerVersion);
PRO_DEF_MEM_DISPATCH(MemberParseServerAddress, parseServerAddress);
PRO_DEF_MEM_DISPATCH(MemberTestServerStartupDone, testServerStartupDone);
PRO_DEF_MEM_DISPATCH(MemberTestRCONStarted, testRCONStarted);
PRO_DEF_MEM_DISPATCH(MemberTestServerStopping, testServerStopping);
} // namespace helium::server::proxy

export namespace helium::server
{
// clang-format off
struct ServerOutputParserFacade : pro::facade_builder
    ::support_copy<pro::constraint_level::trivial>
    ::add_convention<proxy::MemberGetParserName, std::string()>
    ::add_convention<proxy::MemberGetSendMessageCommand, std::string(std::string const &, std::string const&), std::string(std::string const &, utils::rtext::RText const&)>
    ::add_convention<proxy::MemberGetBroadcastMessageCommand, std::string(std::string const &), std::string(utils::rtext::RText const&)>
    ::add_convention<proxy::MemberGetStopCommand, std::string()>
    ::add_convention<proxy::MemberPreprocessServerOutput, std::tuple<std::string, PreprocessedInfo>(std::string const &)>
    ::add_convention<proxy::MemberParseServerOutput, ServerOutputInfo(std::string const &)>
    ::add_convention<proxy::MemberParsePlayerJoined, std::string(std::string const &)>
    ::add_convention<proxy::MemberParsePlayerLeft, std::string(std::string const &)>
    ::add_convention<proxy::MemberParseServerVersion, std::string(std::string const &)>
    ::add_convention<proxy::MemberParseServerAddress, std::string(std::string const &)>
    ::add_convention<proxy::MemberTestServerStartupDone, bool(std::string const &)>
    ::add_convention<proxy::MemberTestRCONStarted, bool(std::string const &)>
    ::add_convention<proxy::MemberTestServerStopping, bool(std::string const &)>
    ::build {};
// clang-format on

using ServerOutputParserProxy = pro::proxy<ServerOutputParserFacade>;
} // namespace helium::server