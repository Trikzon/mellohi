#pragma once

#include "mellohi/core/types.hpp"

namespace mellohi
{
    // TODO: Use a UUID instead.
    using AssetId = u64;
    
    class Asset
    {
    public:
        explicit Asset(AssetId id);
        virtual ~Asset();
        
        void reload();
        
        AssetId id() const;
        
    private:
        const AssetId m_id;
    
        virtual void load() = 0;
    };
    
    class TextAsset : public Asset
    {
    public:
        explicit TextAsset(AssetId id);
        
        const std::string & text() const;
        
    private:
        std::string m_text;
        
        void load() override;
    };
    
    class DependentAsset : public Asset
    {
    public:
        explicit DependentAsset(AssetId id);
        
        const std::string & text() const;
        
    private:
        std::shared_ptr<TextAsset> m_text_asset;
    
        void load() override;
    };
}
