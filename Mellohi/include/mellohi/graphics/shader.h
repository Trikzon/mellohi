#pragma once

#include <string>
#include <unordered_map>

#include "mellohi/asset_id.h"
#include "mellohi/types.h"
#include "mellohi/math/mat4.h"
#include "mellohi/math/vec2.h"
#include "mellohi/math/vec3.h"
#include "mellohi/math/vec4.h"

namespace mellohi
{
    class Shader
    {
    public:
        Shader(const AssetId& fragment_asset_id, const AssetId& vertex_asset_id);
        ~Shader();

        void bind() const;

        void set_uniform_i32(const std::string& name, i32 value);
        void set_uniform_vec2(const std::string& name, const vec2& value);
        void set_uniform_vec3(const std::string& name, const vec3& value);
        void set_uniform_vec4(const std::string& name, const vec4& value);
        void set_uniform_mat4(const std::string& name, const mat4& value);

    private:
        u32 m_program_id;
        std::unordered_map<std::string, i32> m_uniform_locations;

        [[nodiscard]] u32 compile_shader(u32 gl_shader_type, const std::string& shader_source) const;

        i32 get_uniform_location(const std::string& name);
    };
}
