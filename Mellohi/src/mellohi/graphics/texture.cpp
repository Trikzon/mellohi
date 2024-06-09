#include "mellohi/graphics/texture.h"

#include <glad/glad.h>
#include <stb/image.h>

#include "mellohi/log.h"

namespace mellohi
{
    Texture::Texture(const AssetId& asset_id) : m_texture_id(0), m_width(0), m_height(0), m_channels(0)
    {
        MH_ASSERT(asset_id.file_exists(), "Attempted to create a texture with an invalid asset id ({}).", asset_id);

        stbi_set_flip_vertically_on_load(1);
        const auto file_path = asset_id.get_file_path();
        u8* image_buffer = stbi_load(file_path.c_str(), &m_width, &m_height, &m_channels, 4);

        if (image_buffer == nullptr)
        {
            const char* failure_reason = stbi_failure_reason();
            MH_ERROR("Failed to load texture ({}): {}", asset_id, failure_reason);

            return;
        }

        glGenTextures(1, &m_texture_id);
        glBindTexture(GL_TEXTURE_2D, m_texture_id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_buffer);

        stbi_image_free(image_buffer);
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &m_texture_id);
    }

    void Texture::bind(const u32 slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_texture_id);
    }
}
