#pragma once

#include "mellohi/assets/asset.hpp"
#include "mellohi/core/logger.hpp"

#include <shared_mutex>
#include <unordered_set>

namespace mellohi
{
    class AssetCache
    {
    public:
        // Returns the asset with the given id if it exists in the cache.
        template<typename T>
        std::shared_ptr<T> fetch(AssetId id) const;
        // Returns the asset with the given id if it exists in the cache or loads a new one.
        template<typename T>
        std::shared_ptr<T> fetch_or_load(AssetId id);
        // Returns the asset with the given id, loading a new one or re-oading the existing one.
        template<typename T>
        std::shared_ptr<T> load(AssetId id);

        void add_dependency(AssetId dependent_id, AssetId dependency_id);

    private:
        mutable std::shared_mutex m_mutex;
        std::unordered_map<AssetId, std::weak_ptr<Asset>> m_loaded_assets;
        std::unordered_map<AssetId, std::unordered_set<AssetId>> m_dependency_graph;

        void reload_dependents(AssetId dependency_id) const;
        void reload_dependents(AssetId dependency_id, std::unordered_set<AssetId> &reloaded_ids) const;
    };

    template<typename T>
    std::shared_ptr<T> AssetCache::fetch(const AssetId id) const
    {
        std::shared_lock read_lock{m_mutex};
        auto it = m_loaded_assets.find(id);
        if (it == m_loaded_assets.end() || it->second.expired())
        {
            return nullptr;
        }

        const auto t_asset = std::dynamic_pointer_cast<T>(it->second.lock());
        if (!t_asset)
        {
            MH_WARN("Fetched asset {} cannot be cast to requested type.", id);
        }

        return t_asset;
    }

    template<typename T>
    std::shared_ptr<T> AssetCache::fetch_or_load(const AssetId id)
    {
        // First see the asset is already in the cache.
        const auto asset = fetch<T>(id);
        if (asset)
        {
            return asset;
        }

        // Create a new asset outside of a lock to prevent deadlock.
        const auto new_asset = std::make_shared<T>(id);

        {
            std::unique_lock write_lock{m_mutex};

            // Check if an asset has been inserted into the cache between locks.
            auto it = m_loaded_assets.find(id);
            if (it != m_loaded_assets.end() && !it->second.expired())
            {
                const auto t_asset = std::dynamic_pointer_cast<T>(it->second.lock());
                if (!t_asset)
                {
                    MH_WARN("Fetched asset {} cannot be cast to requested type.", id);
                }

                return t_asset;
            }

            // Add the new asset to the cache if we haven't returned by now.
            m_loaded_assets[id] = new_asset;
        }

        reload_dependents(id);

        return new_asset;
    }

    template<typename T>
    std::shared_ptr<T> AssetCache::load(const AssetId id)
    {
        const auto asset = fetch<T>(id);

        // If the asset is not in the cache, simply load a new one.
        if (!asset)
        {
            return fetch_or_load<T>(id);
        }

        // If the asset is in the cache, call "load" on it again. Then notify listeners.
        // This will reload the asset in-place, allowing shared references to still be valid.
        asset->load();

        reload_dependents(id);

        return asset;
    }
}

