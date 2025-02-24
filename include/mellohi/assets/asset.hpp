#pragma once

#include "mellohi/core/types.hpp"

namespace mellohi
{
    class AssetPath;
    
    // TODO: Use a UUID instead.
    using AssetId = u64;
    
    class Asset
    {
    public:
        explicit Asset(AssetId id);
        virtual ~Asset();
        
        AssetId asset_id() const;
        AssetPath asset_path() const;
        
        // Loads (or reloads in-place) the asset.
        // If called directly, will not notify event listeners with the
        // AssetLoadedEvent. Instead, use AssetCache#load(AssetId).
        virtual void load() = 0;
        
    private:
        const AssetId m_id;
    };
    
    class TextAsset : public Asset
    {
    public:
        explicit TextAsset(AssetId id);
        
        void load() override;
        
        const std::string & text() const;
        
    private:
        std::string m_text;
    };
    
    class DependentAsset : public Asset
    {
    public:
        explicit DependentAsset(AssetId id);
        
        void load() override;
        
        const std::string & text() const;
        
    private:
        std::shared_ptr<TextAsset> m_text_asset;
    };
}
