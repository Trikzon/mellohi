#include "mellohi/assets/asset_path.hpp"

namespace mellohi
{
    AssetPath::AssetPath(std::string_view fully_qualified_path)
    {
        m_path = fully_qualified_path;
                
        auto delimiter_pos = m_path.rfind(':');
        if (delimiter_pos != std::string::npos)
        {
            m_package = m_path.substr(0, delimiter_pos);
            m_path = m_path.substr(delimiter_pos + 1);
        }
        
        delimiter_pos = m_package.rfind(':');
        if (delimiter_pos != std::string::npos)
        {
            m_author = m_package.substr(0, delimiter_pos);
            m_package = m_package.substr(delimiter_pos + 1);
        }
    }
    
    AssetPath::AssetPath(std::string_view author, std::string_view package, std::string_view path)
        : m_author(author), m_package(package), m_path(path)
    {
        
    }
    
    bool AssetPath::operator==(const AssetPath &other) const
    {
        return m_author == other.author() && m_package == other.package() && m_path == other.path();
    }
    
    bool AssetPath::operator!=(const AssetPath &other) const
    {
        return !(*this == other);
    }
    
    std::ostream & operator<<(std::ostream &os, const AssetPath &asset_path)
    {
        return os << asset_path.to_fully_qualified_path();
    }
    
    const std::string & AssetPath::author() const
    {
        return m_author;
    }
    
    const std::string & AssetPath::package() const
    {
        return m_package;
    }
    
    const std::string & AssetPath::path() const
    {
        return m_path;
    }
    
    std::string AssetPath::to_fully_qualified_path() const
    {
        return m_author + ":" + m_package + ":" + m_path;
    }
}
