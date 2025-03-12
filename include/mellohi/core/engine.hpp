#pragma once

#include <memory>

namespace mellohi
{
    class AssetCache;
    class AssetRegistry;
    class EngineConfigAsset;
    class EventDispatcher;
    class Graphics;
    class Platform;
    class Window;

    class Engine
    {
    public:
        static Engine & get();

        Engine(const Engine &other) = delete;
        void operator=(const Engine &other) = delete;

        void initialize();
        void run();

        AssetCache & asset_cache();
        AssetRegistry & asset_registry();
        EventDispatcher & event_dispatcher();
        Platform & platform();

        const EngineConfigAsset & engine_config() const;
        Window & main_window();

    private:
        std::unique_ptr<AssetCache> m_asset_cache;
        std::unique_ptr<AssetRegistry> m_asset_registry;
        std::unique_ptr<EventDispatcher> m_event_dispatcher;
        std::unique_ptr<Platform> m_platform;
        std::unique_ptr<Graphics> m_graphics;

        std::shared_ptr<EngineConfigAsset> m_engine_config;
        std::shared_ptr<Window> m_main_window;

        Engine();
        ~Engine() = default;
    };
}

