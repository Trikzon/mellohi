#include "mellohi/assets/asset_registry.hpp"

namespace mellohi
{
    AssetId AssetRegistry::asset_id_from_path(const AssetPath &path)
    {
        std::unique_lock write_lock{m_mutex};
        
        const auto [it, inserted] = m_path_to_id.try_emplace(path, m_next_id);
        if (inserted)
        {
            m_next_id++;
            m_id_to_path.emplace(it->second, it->first);
        }
        
        return it->second;
    }
    
    std::optional<AssetPath> AssetRegistry::asset_path_from_id(AssetId id) const
    {
        std::shared_lock read_lock{m_mutex};
        
        const auto it = m_id_to_path.find(id);
        if (it != m_id_to_path.end())
        {
            return it->second;
        }
        
        return std::nullopt;
    }
}
