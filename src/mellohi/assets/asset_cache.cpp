#include "mellohi/assets/asset_cache.hpp"

namespace mellohi
{
    void AssetCache::add_dependency(const AssetId dependent_id, const AssetId dependency_id)
    {
        {   // First check if the dependency already exists to avoid write locking.
            std::shared_lock read_lock{m_mutex};
            const auto it = m_dependency_graph.find(dependency_id);
            if (it != m_dependency_graph.end() && it->second.contains(dependent_id))
            {
                return;
            }
        }

        {
            std::unique_lock write_lock{m_mutex};
            m_dependency_graph[dependency_id].insert(dependent_id);
        }
    }

    void AssetCache::reload_dependents(AssetId dependency_id) const
    {
        std::unordered_set<AssetId> reloaded_ids;
        reload_dependents(dependency_id, reloaded_ids);
    }

    void AssetCache::reload_dependents(AssetId dependency_id, std::unordered_set<AssetId> &reloaded_ids) const
    {

        reloaded_ids.insert(dependency_id);

        std::unordered_set<AssetId> dependent_ids;

        {
            std::shared_lock read_lock{m_mutex};

            const auto it = m_dependency_graph.find(dependency_id);
            if (it != m_dependency_graph.end())
            {
                dependent_ids = it->second;
            }
        }

        for (const auto &dependent_id : dependent_ids)
        {
            auto dependent = fetch<Asset>(dependent_id);

            if (dependent)
            {
                if (reloaded_ids.find(dependent_id) != reloaded_ids.end())
                {
                    MH_WARN("Asset dependency cycle detected! Skipping reload of asset {}.", dependent_id);
                    return;
                }

                dependent->load();
                reload_dependents(dependent_id, reloaded_ids);
            }
        }

        reloaded_ids.erase(dependency_id);
    }
}

