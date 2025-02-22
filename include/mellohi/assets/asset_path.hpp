#pragma once

#include "mellohi/core/types.hpp"

namespace mellohi
{
    class AssetPath
    {
    public:
        explicit AssetPath(std::string_view fully_qualified_path);
        AssetPath(std::string_view author, std::string_view package, std::string_view path);
        
        bool operator==(const AssetPath &other) const;
        bool operator!=(const AssetPath &other) const;
        friend std::ostream & operator<<(std::ostream &os, const AssetPath &asset_path);
        
        const std::string & author() const;
        const std::string & package() const;
        const std::string & path() const;
        std::string to_fully_qualified_path() const;
        
    private:
        std::string m_author, m_package, m_path;
    };
}

template<>
struct std::hash<mellohi::AssetPath>
{
    mellohi::usize operator()(const mellohi::AssetPath &path) const noexcept
    {
        return std::hash<string>{}(path.to_fully_qualified_path());
    }
};
