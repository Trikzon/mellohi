#pragma once

#include "mellohi/assets/asset.hpp"
#include "mellohi/core/logger.hpp"

#include <toml++/toml.hpp>

namespace mellohi
{
    class Color;
    
    class TomlAsset : public Asset
    {
    public:
        TomlAsset(AssetId id);
        
    protected:
        toml::table parse_toml_table() const;
        
        template<typename T>
        std::optional<T> try_parse(const toml::table &table, std::string_view path) const;
        template<>
        std::optional<AssetPath> try_parse(const toml::table &table, std::string_view path) const;
        template<>
        std::optional<AssetId> try_parse(const toml::table &table, std::string_view path) const;
        template<>
        std::optional<Color> try_parse(const toml::table &table, std::string_view path) const;
        template<>
        std::optional<uvec2> try_parse(const toml::table &table, std::string_view path) const;
        
        template<typename T>
        T parse(const toml::table &table, std::string_view path, std::string_view type_name) const;
    };
    
    template<typename T>
    std::optional<T> TomlAsset::try_parse(const toml::table &table, std::string_view path) const
    {
        return table[toml::path(path)].value<T>();
    }
    
    template<typename T>
    T TomlAsset::parse(const toml::table &table, std::string_view path, std::string_view type_name) const
    {
        const auto value_opt = try_parse<T>(table, path);
        MH_ASSERT(value_opt.has_value(), "Toml asset {} is missing {} {}.", asset_id(), type_name, path);
        return value_opt.value();
    }
}
