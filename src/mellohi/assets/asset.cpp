#include "mellohi/assets/asset.hpp"

#include "mellohi/assets/asset_cache.hpp"
#include "mellohi/assets/asset_registry.hpp"
#include "mellohi/core/engine.hpp"

namespace mellohi
{
    Asset::Asset(AssetId id) : m_id(id)
    {
        MH_TRACE("Constructing asset {}.", id);
    }

    Asset::~Asset()
    {
        MH_TRACE("Destructing asset {}.", asset_id());
    }

    AssetId Asset::asset_id() const
    {
        return m_id;
    }

    AssetPath Asset::asset_path() const
    {
        const auto path_opt = Engine::get().asset_registry().asset_path_from_id(asset_id());
        MH_ASSERT(path_opt.has_value(), "Asset {} is not registered in the AssetRegistry.", asset_id());
        return path_opt.value();
    }

    TextAsset::TextAsset(AssetId id) : Asset(id)
    {
        load();
    }

    void TextAsset::load()
    {
        MH_TRACE("Loading asset {}.", asset_id());
        m_text = "Hello, world!";
    }

    const std::string & TextAsset::text() const
    {
        return m_text;
    }

    DependentAsset::DependentAsset(AssetId id) : Asset(id)
    {
        load();
    }

    void DependentAsset::load()
    {
        MH_TRACE("Loading asset {}.", asset_id());
        auto &registry = Engine::get().asset_registry();
        auto &cache = Engine::get().asset_cache();

        const AssetPath path{"@core:game:config.toml"};

        m_text_asset = cache.fetch<TextAsset>(registry.asset_id_from_path(path));
    }

    const std::string & DependentAsset::text() const
    {
        return m_text_asset->text();
    }
}

