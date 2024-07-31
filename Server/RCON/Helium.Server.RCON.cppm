/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <optional>
#include <string>

export module Helium.Server.RCON;

import Helium.Base;
import Helium.Logger;

export namespace helium::server
{
class RCONConnection final : public base::HeliumObject
{
private:
    std::string address;
    int port;
    std::string password;

public:
    explicit RCONConnection(std::string address, int port, std::string password)
        : address(std::move(address)), port(port), password(std::move(password))
    {
    }

    auto connect() -> bool
    {
    }

    auto disconnent() -> bool
    {
    }

    auto send_command(std::string const &message, int max_retry_time) -> std::optional<std::string>
    {
    }
};
} // namespace helium::server