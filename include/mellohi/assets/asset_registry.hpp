#pragma once 

#include "mellohi/assets/asset.hpp"
#include "mellohi/assets/asset_path.hpp"

#include <shared_mutex>

namespace mellohi {
    class AssetRegistry
    {
    public:
        AssetId asset_id_from_path(const AssetPath &path);
        std::optional<AssetPath> asset_path_from_id(AssetId id) const;

    private:
        mutable std::shared_mutex m_mutex;
        u64 m_next_id;
        std::unordered_map<AssetPath, AssetId> m_path_to_id;
        std::unordered_map<AssetId, AssetPath> m_id_to_path;
    };
}

