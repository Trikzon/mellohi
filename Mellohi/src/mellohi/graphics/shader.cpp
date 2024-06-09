#include "mellohi/graphics/shader.h"

#include <glad/glad.h>

#include "mellohi/log.h"

namespace mellohi
{
    // Requires that the vao is bound for validation.
    Shader::Shader(const std::string& vertex_shader_source, const std::string& fragment_shader_source)
    {
        m_program_id = glCreateProgram();

        const u32 vertex_shader_id = compile_shader(GL_VERTEX_SHADER, vertex_shader_source);
        const u32 fragment_shader_id = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_source);

        glAttachShader(m_program_id, vertex_shader_id);
        glAttachShader(m_program_id, fragment_shader_id);
        glLinkProgram(m_program_id);

        i32 link_status;
        glGetProgramiv(m_program_id, GL_LINK_STATUS, &link_status);
        if (link_status == GL_FALSE)
        {
            i32 info_log_length;
            glGetProgramiv(m_program_id, GL_INFO_LOG_LENGTH, &info_log_length);

            const auto info_log = new char[info_log_length];
            glGetProgramInfoLog(m_program_id, info_log_length, &info_log_length, info_log);
            MH_ERROR("Failed to link shader program (id: {}).", m_program_id);
            MH_ERROR("Info Log:\n{}", info_log);

            delete [] info_log;
        }

        glValidateProgram(m_program_id);

        i32 validate_status;
        glGetProgramiv(m_program_id, GL_VALIDATE_STATUS, &validate_status);
        if (validate_status == GL_FALSE)
        {
            i32 info_log_length;
            glGetProgramiv(m_program_id, GL_INFO_LOG_LENGTH, &info_log_length);

            const auto info_log = new char[info_log_length];
            glGetProgramInfoLog(m_program_id, info_log_length, &info_log_length, info_log);
            MH_ERROR("Failed to validate shader program (id: {}).", m_program_id);
            MH_ERROR("Info Log:\n{}", info_log);

            delete [] info_log;
        }

        glDetachShader(m_program_id, vertex_shader_id);
        glDetachShader(m_program_id, fragment_shader_id);
        glDeleteShader(vertex_shader_id);
        glDeleteShader(fragment_shader_id);
    }

    Shader::~Shader()
    {
        glDeleteProgram(m_program_id);
    }

    void Shader::bind() const
    {
        glUseProgram(m_program_id);
    }

    void Shader::set_uniform_i32(const std::string &name, const i32 value)
    {
        glUniform1i(get_uniform_location(name), value);
    }

    void Shader::set_uniform_vec2(const std::string &name, const vec2& value)
    {
        glUniform2f(get_uniform_location(name), value.x, value.y);
    }

    void Shader::set_uniform_vec3(const std::string &name, const vec3& value)
    {
        glUniform3f(get_uniform_location(name), value.x, value.y, value.z);
    }

    void Shader::set_uniform_vec4(const std::string &name, const vec4& value)
    {
        glUniform4f(get_uniform_location(name), value.x, value.y, value.z, value.w);
    }

    void Shader::set_uniform_mat4(const std::string &name, const mat4& value)
    {
        glUniformMatrix4fv(get_uniform_location(name), 1, GL_TRUE, &value[0][0]);
    }

    u32 Shader::compile_shader(const u32 gl_shader_type, const std::string& shader_source) const
    {
        const u32 shader_id = glCreateShader(gl_shader_type);

        const char* src = shader_source.c_str();
        glShaderSource(shader_id, 1, &src, nullptr);
        glCompileShader(shader_id);

        i32 compile_status;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status);
        if (compile_status == GL_FALSE)
        {
            i32 info_log_length;
            glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);

            const auto info_log = new char[info_log_length];
            glGetShaderInfoLog(shader_id, info_log_length, &info_log_length, info_log);

            const auto shader_type = gl_shader_type == GL_VERTEX_SHADER ? "vertex" : "fragment";
            MH_ERROR("Failed to compile {} shader on program (id: {}).", shader_type, m_program_id);
            MH_ERROR("Info Log:\n{}", info_log);

            delete [] info_log;

            glDeleteShader(shader_id);
            return 0;
        }

        return shader_id;
    }

    i32 Shader::get_uniform_location(const std::string &name)
    {
        const auto cached_location = m_uniform_locations.find(name);
        if (cached_location != m_uniform_locations.end())
        {
            return cached_location->second;
        }

        const i32 location = glGetUniformLocation(m_program_id, name.c_str());

        if (location == -1)
        {
            MH_WARN("Uniform (name: {}) does not exist on shader program (id: {}).", name, m_program_id);
        }

        m_uniform_locations[name] = location;

        return location;
    }
}
