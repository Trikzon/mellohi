#pragma once

#include "mellohi/assets/asset.hpp"
#include "mellohi/core/logger.hpp"

#include <shared_mutex>

namespace mellohi
{
    class AssetCache
    {
    public:
        template<typename T>
        std::shared_ptr<T> fetch(AssetId id);
        
    private:
        mutable std::shared_mutex m_mutex;
        std::unordered_map<AssetId, std::weak_ptr<Asset>> m_loaded_assets;
    };
    
    template<typename T>
    std::shared_ptr<T> AssetCache::fetch(AssetId id)
    {
        std::shared_ptr<Asset> base_asset;
        
        // First check if the asset is already in the cache.
        {
            std::shared_lock read_lock{m_mutex};
            auto it = m_loaded_assets.find(id);
            if (it != m_loaded_assets.end())
            {
                base_asset = it->second.lock();
            }
        }
        
        // If it is not in the cache, create it.
        if (!base_asset)
        {
            // We create it outside of locks to avoid deadlock.
            const auto new_asset = std::make_shared<T>(id);
            
            {
                std::unique_lock write_lock{m_mutex};
                
                auto &weak_base_asset = m_loaded_assets[id];
                base_asset = weak_base_asset.lock();
                
                // Only return the new one if it's still not in the cache.
                if (!base_asset)
                {
                    weak_base_asset = new_asset;
                    return new_asset;
                }
            }
        }
        
        // If we haven't returned a new one yet, cast to T and return it.
        const auto t_asset = std::dynamic_pointer_cast<T>(base_asset);
        if (!t_asset)
        {
            MH_WARN("Fetched asset {} cannot be cast to requested type.", id);
        }
        return t_asset;
    }
}
