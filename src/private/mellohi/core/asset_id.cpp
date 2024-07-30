#include "mellohi/core/asset_id.hpp"

#include <fstream>

#include "mellohi/core/log.hpp"

namespace mellohi
{
    AssetId::AssetId(str fully_qualified_id)
    {
        const usize delimiter_pos = fully_qualified_id.find(':');

        MH_ASSERT(delimiter_pos != str::npos, "AssetId must be fully qualified with a namespace.");

        m_namespace = fully_qualified_id.substr(0, delimiter_pos);
        m_path = fully_qualified_id.substr(delimiter_pos + 1);

        if (!file_exists())
        {
            MH_WARN("AssetId '{}' does not point to an existing file.", *this);
        }
    }

    AssetId::AssetId(const str namespace_, const str id) : m_namespace(namespace_), m_path(id)
    {
        if (!file_exists())
        {
            MH_WARN("AssetId '{}' does not point to an existing file.", *this);
        }
    }

    auto AssetId::operator==(const AssetId &other) const -> bool
    {
        return m_namespace == other.m_namespace && m_path == other.m_path;
    }

    auto AssetId::operator!=(const AssetId &other) const -> bool
    {
        return !(*this == other);
    }

    auto operator<<(std::ostream &os, const AssetId &asset_id) -> std::ostream &
    {
        return os << asset_id.get_fully_qualified_id();
    }

    auto AssetId::get_fully_qualified_id() const -> string
    {
        return m_namespace + ":" + m_path;
    }

    auto AssetId::get_namespace() const -> const string &
    {
        return m_namespace;
    }

    auto AssetId::get_id() const -> const string &
    {
        return m_path;
    }

    auto AssetId::file_exists() const -> bool
    {
        return exists(get_file_path());
    }

    auto AssetId::get_file_path() const -> path
    {
        return path("assets") / m_namespace / m_path;
    }

    auto AssetId::read_file_to_string() const -> string
    {
        MH_ASSERT(file_exists(), "Cannot read file to string because the file does not exist.");

        std::ifstream ifs(get_file_path());
        std::string content((std::istreambuf_iterator(ifs)), (std::istreambuf_iterator<char>()));
        return content;
    }
}
