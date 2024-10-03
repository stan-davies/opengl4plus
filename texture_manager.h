#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

bool create_texture(const char *path, GLuint *texture_id, GLenum channels) {
        // texture time
        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        // as we chose clamp to border, we must also set a border colour
        float border_colour[] = { 1.f, 1.f, 0.f, 1.f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_colour);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // loading and creating texture from image
        int img_width;
        int img_height;
        int img_channels;
        unsigned char *img_data = stbi_load(path, &img_width, &img_height, &img_channels, 0);
        if (!img_data) {
                log_err("ERROR: image could not be loaded");
                stbi_image_free(img_data);
                return false;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, channels, GL_UNSIGNED_BYTE, img_data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(img_data);

        *texture_id = tex;
        return true;
}

#endif