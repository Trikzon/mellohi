#include "mellohi/core/engine.hpp"

#include "mellohi/assets/asset_cache.hpp"
#include "mellohi/assets/asset_registry.hpp"
#include "mellohi/core/config_assets.hpp"
#include "mellohi/events/event_dispatcher.hpp"
#include "mellohi/graphics/graphics.hpp"
#include "mellohi/platform/platform.hpp"

namespace mellohi
{
    Engine & Engine::get()
    {
        static Engine instance;
        return instance;
    }

    void Engine::initialize()
    {
        const auto engine_config_id = asset_registry().asset_id_from_path(AssetPath{"@:config:engine.toml"});
        m_engine_config = asset_cache().load<EngineConfigAsset>(engine_config_id);

        m_main_window = m_platform->create_window(
            m_engine_config->window_initial_size(),
            m_engine_config->window_resizable(),
            m_engine_config->window_title()
        );
    }

    void Engine::run()
    {
        while (!m_main_window->should_close())
        {
            m_platform->process_events();

            m_graphics->draw_frame();
        }
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

    Platform & Engine::platform()
    {
        return *m_platform;
    }

    const EngineConfigAsset & Engine::engine_config() const
    {
        MH_ASSERT(m_engine_config != nullptr, "Engine config is not available as Engine has not been initialized.");
        return *m_engine_config;
    }

    Engine::Engine()
    {
        m_asset_cache = std::make_unique<AssetCache>();
        m_asset_registry = std::make_unique<AssetRegistry>();
        m_event_dispatcher = std::make_unique<EventDispatcher>();
        m_platform = std::make_unique<Platform>();
        m_graphics = std::make_unique<Graphics>(*m_platform);
    }
}

