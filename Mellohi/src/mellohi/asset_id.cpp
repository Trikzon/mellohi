#include "mellohi/asset_id.h"

#include <filesystem>
#include <fstream>

#include "mellohi/log.h"

namespace mellohi
{
    AssetId::AssetId(const std::string_view& namespace_, const std::string_view& path)
        : m_namespace(namespace_), m_path(path)
    {
        if (!file_exists())
        {
            MH_WARN("ResourceId identifies a file that does not exist ({}).", get_file_path());
        }
    }

    AssetId::AssetId(const std::string_view &path) : AssetId(m_default_namespace, path)
    {
        MH_ASSERT(!m_default_namespace.empty(), "Attempted to create AssetId with default namespace before setting a default namespace.");
    }


    void AssetId::set_default_namespace(const std::string_view &default_namespace)
    {
        m_default_namespace = default_namespace;
    }

    const std::string& AssetId::get_namespace() const
    {
        return m_namespace;
    }

    const std::string& AssetId::get_path() const
    {
        return m_path;
    }

    std::string AssetId::get_file_path() const
    {
        return "assets/" + m_namespace + "/" + m_path;
    }

    bool AssetId::file_exists() const
    {
        return std::filesystem::exists(get_file_path());
    }

    std::string AssetId::read_file_to_string() const
    {
        std::ifstream ifs(get_file_path());
        std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        return content;
    }

    std::ostream& operator<<(std::ostream& os, const AssetId& asset_id)
    {
        return os << std::format("{}", asset_id);
    }
}
