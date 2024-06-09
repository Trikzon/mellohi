#pragma once

#include <format>
#include <iostream>
#include <string>

namespace mellohi
{
    class AssetId
    {
    public:
        AssetId(const std::string_view& namespace_, const std::string_view& path);
        explicit AssetId(const std::string_view& path);

        static void set_default_namespace(const std::string_view& default_namespace);

        const std::string& get_namespace() const;
        const std::string& get_path() const;

        std::string get_file_path() const;

        bool file_exists() const;

        std::string read_file_to_string() const;

    private:
        inline static std::string m_default_namespace;

        std::string m_namespace;
        std::string m_path;

        friend std::ostream& operator<<(std::ostream& os, const AssetId& res_id);
    };
}

template<>
struct std::formatter<mellohi::AssetId> : std::formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const mellohi::AssetId& res_id, FormatContext& ctx) const
    {
        return std::format_to(ctx.out(), "{}:{}", res_id.get_namespace(), res_id.get_path());
    }
};
