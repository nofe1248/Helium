/*
 * Helium is an open source software distributed under the MIT license.
 * Please refer to Helium.Main.cppm for full license info.
 */

module;

#include <compare>
#include <optional>
#include <string>
#include <variant>

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>

export module Helium.Server.ServerOutputParser.ServerOutputInfo;

namespace uuids = boost::uuids;

export namespace helium::server
{
struct ServerOutputInfoTimeStamp
{
    int hour;
    int minute;
    int second;
};
struct PreprocessedInfo
{
    ServerOutputInfoTimeStamp timestamp;
    std::string log_level;
};
enum class ServerOutputInfoType
{
    SERVER_OUTPUT,
    PLAYER_MESSAGE,
    PLAYER_JOINED,
    PLAYER_LEFT,
    SERVER_STARTED,
    SERVER_STOPPING,
    SERVER_ADDRESS,
    SERVER_VERSION,
    RCON_STARTED,
    CUSTOM_INFO_TYPE
};
struct PlayerMessage
{
    bool is_secure = false;
    std::string player_name;
    std::string player_message;
};
struct Position
{
    double x = 0.0f;
    double y = 0.0f;
    double z = 0.0f;
};
struct IPAddress
{
    std::string ip;
    int port;
};
struct PlayerJoined
{
    std::string player_name;
    int entity_id = 0;
    Position position;
    std::optional<IPAddress> address = std::nullopt;
};
struct PlayerLeft
{
    std::string player_name;
};
struct ServerAddress
{
    std::string ip;
    int port = 0;
};
struct ServerVersion
{
    std::string version;
};
struct ServerOutputInfo
{
    uuids::uuid id;

    ServerOutputInfoType info_type;
    std::optional<std::string> extra_info_type;

    std::string raw_content;
    std::string content;

    PreprocessedInfo preprocessed_info;

    using InfoType = std::variant<PlayerJoined, PlayerLeft, PlayerMessage, ServerAddress, ServerVersion>;
    std::optional<InfoType> info = std::nullopt;

    explicit ServerOutputInfo(ServerOutputInfoType const info_type, PreprocessedInfo preprocessed_info, std::string raw_content, std::string content,
                              std::optional<InfoType> info = std::nullopt, std::optional<std::string> extra_info_type = std::nullopt)
        : id(uuids::random_generator()()), info_type(info_type), extra_info_type(std::move(extra_info_type)), raw_content(std::move(raw_content)),
          content(std::move(content)), preprocessed_info(std::move(preprocessed_info)), info(std::move(info))
    {
    }

    ServerOutputInfo(ServerOutputInfo const &) = default;
    ServerOutputInfo(ServerOutputInfo &&) noexcept = default;
    auto operator=(ServerOutputInfo const &) -> ServerOutputInfo & = default;
    auto operator=(ServerOutputInfo &&) noexcept -> ServerOutputInfo & = default;

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