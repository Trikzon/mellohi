#include "mellohi/core/config_assets.hpp"

#include "mellohi/assets/asset_cache.hpp"
#include "mellohi/assets/asset_registry.hpp"
#include "mellohi/core/engine.hpp"

namespace mellohi
{
    GameConfigAsset::GameConfigAsset(AssetId id) : TomlAsset(id)
    {
        const auto path = asset_path(), desired_path = AssetPath{"@:config:game.toml"};
        MH_ASSERT(path == desired_path, "Game config must be at {}, not {}.", desired_path, path);
        
        load();
    }
            
    void GameConfigAsset::load()
    {
        MH_TRACE("Loading asset {}.", asset_id());
        
        auto table = parse_toml_table();
        
        {
            std::unique_lock write_lock{m_mutex};
            
            m_game.package = parse<std::string>(table, "game.package", "string");
            m_game.title = parse<std::string>(table, "game.title", "string");
            
            m_window.clear_color = try_parse<Color>(table, "window.clear_color");
            m_window.initial_size = try_parse<uvec2>(table, "window.initial_size");
            m_window.resizable = try_parse<bool>(table, "window.resizable");
            m_window.title = try_parse<std::string>(table, "window.title");
            m_window.vsync = try_parse<bool>(table, "window.vsync");
        }
    }
    
    std::string GameConfigAsset::game_package() const
    {
        std::shared_lock read_lock{m_mutex};
        return m_game.package;
    }
        
    std::string GameConfigAsset::game_title() const
    {
        std::shared_lock read_lock{m_mutex};
        return m_game.title;
    }
    
    std::optional<Color> GameConfigAsset::window_clear_color() const
    {
        std::shared_lock read_lock{m_mutex};
        return m_window.clear_color;
    }
    
    std::optional<uvec2> GameConfigAsset::window_initial_size() const
    {
        std::shared_lock read_lock{m_mutex};
        return m_window.initial_size;
    }
    
    std::optional<bool> GameConfigAsset::window_resizable() const
    {
        std::shared_lock read_lock{m_mutex};
        return m_window.resizable;
    }
    
    std::optional<std::string> GameConfigAsset::window_title() const
    {
        std::shared_lock read_lock{m_mutex};
        return m_window.title;
    }
    
    std::optional<bool> GameConfigAsset::window_vsync() const
    {
        std::shared_lock read_lock{m_mutex};
        return m_window.vsync;
    }
    
    EngineConfigAsset::EngineConfigAsset(AssetId id) : TomlAsset(id)
    {
        const auto path = asset_path(), desired_path = AssetPath{"@:config:engine.toml"};
        MH_ASSERT(path == desired_path, "Engine config must be at {}, not {}.", desired_path, path);
        
        m_game_config_id = Engine::get().asset_registry().asset_id_from_path(AssetPath{"@:config:game.toml"});
        Engine::get().asset_cache().add_dependency(id, m_game_config_id);
        
        load();
    }
    
    void EngineConfigAsset::load()
    {
        MH_TRACE("Loading asset {}.", asset_id());
        
        const auto game_config = Engine::get().asset_cache().fetch_or_load<GameConfigAsset>(m_game_config_id);
        MH_ASSERT(game_config != nullptr, "Engine config requires game config to be valid.");
        
        auto table = parse_toml_table();
        
        {
            std::unique_lock write_lock{m_mutex};
            
            m_engine.package = parse<std::string>(table, "engine.package", "string");
            m_engine.title = parse<std::string>(table, "engine.title", "string");
            
            m_game.package = game_config->game_package();
            m_game.title = game_config->game_title();
            
            m_window.clear_color = game_config->window_clear_color()
                .or_else([&] { return std::optional{parse<Color>(table, "window.clear_color", "string")}; })
                .value();
            m_window.initial_size = game_config->window_initial_size()
                .or_else([&] { return std::optional{parse<uvec2>(table, "window.initial_size", "uvec2")}; })
                .value();
            m_window.resizable = game_config->window_resizable()
                .or_else([&] { return std::optional{parse<bool>(table, "window.resizable", "bool")}; })
                .value();
            m_window.title = game_config->window_title()
                .or_else([&] { return std::optional{parse<std::string>(table, "window.title", "string")}; })
                .value();
            m_window.vsync = game_config->window_vsync()
                .or_else([&] { return std::optional{parse<bool>(table, "window.vsync", "bool")}; })
                .value();
        }
    }
    
    std::string EngineConfigAsset::engine_package() const
    {
        std::shared_lock read_lock{m_mutex};
        return m_engine.package;
    }
    
    std::string EngineConfigAsset::engine_title() const
    {
        std::shared_lock read_lock{m_mutex};
        return m_engine.title;
    }
    
    std::string EngineConfigAsset::game_package() const
    {
        std::shared_lock read_lock{m_mutex};
        return m_game.package;
    }
    
    std::string EngineConfigAsset::game_title() const
    {
        std::shared_lock read_lock{m_mutex};
        return m_game.title;
    }
    
    Color EngineConfigAsset::window_clear_color() const
    {
        std::shared_lock read_lock{m_mutex};
        return m_window.clear_color;
    }
    
    uvec2 EngineConfigAsset::window_initial_size() const
    {
        std::shared_lock read_lock{m_mutex};
        return m_window.initial_size;
    }
    
    bool EngineConfigAsset::window_resizable() const
    {
        std::shared_lock read_lock{m_mutex};
        return m_window.resizable;
    }
    
    std::string EngineConfigAsset::window_title() const
    {
        std::shared_lock read_lock{m_mutex};
        return m_window.title;
    }
    
    bool EngineConfigAsset::window_vsync() const
    {
        std::shared_lock read_lock{m_mutex};
        return m_window.vsync;
    }
}
