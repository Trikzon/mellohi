#include "mellohi/core/engine.hpp"

namespace mellohi
{
    Engine & Engine::get()
    {
        static Engine instance;
        return instance;
    }
    
    AssetCache & Engine::asset_cache()
    {
        return *m_asset_cache;
    }
    
    AssetRegistry & Engine::asset_registry()
    {
        return *m_asset_registry;
    }
    
    EventDispatcher & Engine::event_dispatcher()
    {
        return *m_event_dispatcher;
    }
    
    Engine::Engine()
    {
        m_asset_cache = std::make_unique<AssetCache>();
        m_asset_registry = std::make_unique<AssetRegistry>();
        m_event_dispatcher = std::make_unique<EventDispatcher>();
    }
}
