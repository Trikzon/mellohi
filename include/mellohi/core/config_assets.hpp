#pragma once

#include "mellohi/core/color.hpp"
#include "mellohi/assets/toml_asset.hpp"

#include <shared_mutex>

namespace mellohi
{
    class GameConfigAsset : public TomlAsset
    {
    public:
        explicit GameConfigAsset(AssetId id);

        void load() override;

        std::string game_package() const;
        std::string game_title() const;

        std::optional<Color> window_clear_color() const;
        std::optional<uvec2> window_initial_size() const;
        std::optional<bool> window_resizable() const;
        std::optional<std::string> window_title() const;
        std::optional<bool> window_vsync() const;

    private:
        mutable std::shared_mutex m_mutex;

        struct
        {
            std::string package;
            std::string title;
        } m_game{};

        struct
        {
            std::optional<Color> clear_color;
            std::optional<uvec2> initial_size;
            std::optional<bool> resizable;
            std::optional<std::string> title;
            std::optional<bool> vsync;
        } m_window{};
    };

    class EngineConfigAsset : public TomlAsset
    {
    public:
        explicit EngineConfigAsset(AssetId id);

        void load() override;

        std::string engine_package() const;
        std::string engine_title() const;

        std::string game_package() const;
        std::string game_title() const;

        Color window_clear_color() const;
        uvec2 window_initial_size() const;
        bool window_resizable() const;
        std::string window_title() const;
        bool window_vsync() const;

    private:
        mutable std::shared_mutex m_mutex;

        AssetId m_game_config_id;

        struct
        {
            std::string package;
            std::string title;
        } m_engine{};

        struct
        {
            std::string package;
            std::string title;
        } m_game{};

        struct
        {
            Color clear_color;
            uvec2 initial_size;
            bool resizable;
            std::string title;
            bool vsync;
        } m_window{};
    };
}

