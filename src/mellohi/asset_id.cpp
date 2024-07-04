#include "mellohi/asset_id.hpp"

#include <fstream>

namespace mellohi
{
    AssetId AssetId::create(const std::string_view &namespace_, const std::string_view &path)
    {
        return { namespace_, path };
    }

    AssetId AssetId::fromEngine(const std::string_view &path)
    {
        return { ENGINE_NAMESPACE, path };
    }

    AssetId AssetId::fromGame(const std::string_view &path)
    {
        // TODO: Log if the game namespace hasn't been set.
        return { s_game_namespace, path };
    }

    void AssetId::set_game_namespace(const std::string_view &game_namespace)
    {
        s_game_namespace = game_namespace;
    }

    const std::string & AssetId::get_namespace() const
    {
        return m_namespace;
    }

    const std::string & AssetId::get_path() const
    {
        return m_path;
    }

    std::string AssetId::get_file_path() const
    {
        return "./assets/" + m_namespace + "/" + m_path;
    }

    bool AssetId::file_exists() const
    {
        return std::filesystem::exists(get_file_path());
    }

    std::string AssetId::read_file_to_string() const
    {
        // TODO: Log if the file doesn't exist and return "".
        std::ifstream ifs(get_file_path());
        std::string content((std::istreambuf_iterator(ifs)), (std::istreambuf_iterator<char>()));
        return content;
    }

    AssetId::AssetId(const std::string_view &namespace_, const std::string_view &path)
        : m_namespace(namespace_), m_path(path) { }
}
