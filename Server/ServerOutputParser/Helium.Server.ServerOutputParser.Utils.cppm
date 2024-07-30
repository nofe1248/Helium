/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <compare>
#include <optional>
#include <string>

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>

export module Helium.Server.ServerOutputParser.Utils;

namespace uuids = boost::uuids;

export namespace helium::server
{
struct ServerOutputInfoTimeStamp
{
    int hour;
    int minute;
    int second;
};
struct ServerOutputInfo
{
    uuids::uuid id;

    ServerOutputInfoTimeStamp timestamp;

    std::string raw_content;
    std::string content;

    std::string log_level;

    std::optional<std::string> player_name;

    explicit ServerOutputInfo(ServerOutputInfoTimeStamp timestamp, std::string raw_content, std::string content, std::string log_level,
                              std::optional<std::string> player_name = std::nullopt)
        : id(uuids::random_generator()()), timestamp(timestamp), raw_content(std::move(raw_content)), content(std::move(content)),
          log_level(std::move(log_level)), player_name(std::move(player_name))
    {
    }

    ServerOutputInfo(ServerOutputInfo const &) = default;
    ServerOutputInfo(ServerOutputInfo &&) noexcept = default;
    auto operator=(ServerOutputInfo const &) -> ServerOutputInfo& = default;
    auto operator=(ServerOutputInfo &&) noexcept -> ServerOutputInfo& = default;

    friend auto operator<=>(ServerOutputInfo const &lhs, ServerOutputInfo const &rhs) -> std::strong_ordering
    {
        if (lhs.id == rhs.id)
        {
            return std::strong_ordering::equal;
        }
        if (lhs.id < rhs.id)
        {
            return std::strong_ordering::less;
        }
        if (lhs.id > rhs.id)
        {
            return std::strong_ordering::greater;
        }
        std::unreachable();
    }
};
} // namespace helium::server