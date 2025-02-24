#include "mellohi/assets/toml_asset.hpp"

#include "mellohi/assets/asset_registry.hpp"
#include "mellohi/core/engine.hpp"

namespace mellohi
{
    TomlAsset::TomlAsset(AssetId id) : Asset(id)
    {
        
    }
    
    toml::table TomlAsset::parse_toml_table() const
    {
        return toml::parse(asset_path().read_file_to_string());
    }
    
    template<>
    std::optional<AssetPath> TomlAsset::try_parse(const toml::table &table, std::string_view path) const
    {
        return try_parse<std::string>(table, path).transform([](const auto &str) { return AssetPath{str}; });
    }
    
    template<>
    std::optional<AssetId> TomlAsset::try_parse(const toml::table &table, const std::string_view path) const
    {
        return try_parse<AssetPath>(table, path).transform([](const auto &path)
        {
            return Engine::get().asset_registry().asset_id_from_path(path);
        });
    }
    
    template<>
    std::optional<Color> TomlAsset::try_parse(const toml::table &table, const std::string_view path) const
    {
        const auto value_array_ptr = table[toml::path(path)].as_array();
        if (value_array_ptr && value_array_ptr->size() >= 3 && value_array_ptr->size() <= 4)
        {
            const auto value_r_opt = value_array_ptr->at(0).value<f32>();
            const auto value_g_opt = value_array_ptr->at(1).value<f32>();
            const auto value_b_opt = value_array_ptr->at(2).value<f32>();
            
            if (value_r_opt.has_value() && value_g_opt.has_value() && value_b_opt.has_value())
            {
                const auto value_a_ptr = value_array_ptr->get(3);
                
                if (value_a_ptr)
                {
                    const auto value_a_opt = value_a_ptr->value<f32>();
                    
                    if (value_a_opt.has_value())
                    {
                        return Color{value_r_opt.value(), value_g_opt.value(), value_b_opt.value(), value_a_opt.value()};
                    }
                }
                
                return Color{value_r_opt.value(), value_g_opt.value(), value_b_opt.value()};
            }
        }
        
        const auto value_string_opt = table[toml::path(path)].value<std::string>();
        if (value_string_opt.has_value())
        {
            return Color(value_string_opt.value());
        }
        
        return std::nullopt;
    }
    
    template<>
    std::optional<uvec2> TomlAsset::try_parse(const toml::table &table, const std::string_view path) const
    {
        const auto value_arr_ptr = table[toml::path(path)].as_array();
        if (value_arr_ptr && value_arr_ptr->size() == 2)
        {
            const auto value_x_opt = value_arr_ptr->at(0).value<u32>();
            const auto value_y_opt = value_arr_ptr->at(1).value<u32>();
            
            if (value_x_opt.has_value() && value_y_opt.has_value())
            {
                return uvec2{value_x_opt.value(), value_y_opt.value()};
            }
        }
        
        return std::nullopt;
    }
}
