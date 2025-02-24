#pragma once

#include "mellohi/assets/asset_cache.hpp"
#include "mellohi/assets/asset_registry.hpp"
#include "mellohi/events/event_dispatcher.hpp"

namespace mellohi
{
    class Engine
    {
    public:
        static Engine & get();
        
        Engine(const Engine &other) = delete;
        void operator=(const Engine &other) = delete;
        
        AssetCache & asset_cache();
        AssetRegistry & asset_registry();
        EventDispatcher & event_dispatcher();
    
    private:
        std::unique_ptr<AssetCache> m_asset_cache;
        std::unique_ptr<AssetRegistry> m_asset_registry;
        std::unique_ptr<EventDispatcher> m_event_dispatcher;
    
        Engine();
        ~Engine() = default;
    };
}
