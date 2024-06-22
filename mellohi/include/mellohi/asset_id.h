#pragma once

#include <string>

namespace mellohi
{
    class AssetId
    {
    public:
        static AssetId create(const std::string_view& namespace_, const std::string_view& path);
        static AssetId fromEngine(const std::string_view& path);
        static AssetId fromGame(const std::string_view& path);

        static void set_game_namespace(const std::string_view& game_namespace);

        const std::string& get_namespace() const;
        const std::string& get_path() const;

        std::string get_file_path() const;

        bool file_exists() const;

        std::string read_file_to_string() const;

    private:
        const inline static std::string ENGINE_NAMESPACE = "mellohi";
        inline static std::string GAME_NAMESPACE;

        const std::string m_namespace, m_path;

        AssetId(const std::string_view& namespace_, const std::string_view& path);
    };
}
