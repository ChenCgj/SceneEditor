#include <iostream>
#include "SDL_image.h"
#include "shader.h"
#include "texture_manager.h"
#include "debug.h"

Texture_manager Texture_manager::inst;
Texture_manager::Texture_manager() : max_texture(16), texture_map(max_texture, std::make_pair(0, GL_TEXTURE_2D))
{
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_WEBP);
}

Texture_manager::~Texture_manager()
{
    IMG_Quit();
}

Texture_manager *Texture_manager::instance()
{
    return &inst;
}

GLuint Texture_manager::load_texture(GLenum texture_type, const std::string &image, GLenum min_filter, GLenum mag_filter, GLenum wrap_s, GLenum wrap_t, float r, float g, float b, float a)
{
    SDL_Surface *img_surface = IMG_Load(image.c_str());
    if (img_surface == nullptr) {
        ERRINFO("Load image fail. SDL_image: %s", IMG_GetError());
        return 0;
    }
    int width = img_surface->w;
    int height = img_surface->h;
    int pitch = img_surface->pitch;
    const char *data = static_cast<const char *>(img_surface->pixels);
    char *buf = new char[pitch * height];
    if (buf == nullptr) {
        ERRINFO("Load image fail: no enought memory");
        SDL_FreeSurface(img_surface);
        return -1;
    }
    for (int i = 0; i < height; ++i) {
        memcpy(buf + pitch * (height - i - 1), data + pitch * i, pitch);
    }

    GLenum form;
    int depth = pitch / width;
    if (depth == 1) {
        form = GL_RED;
    } else if (depth == 3) {
        form = GL_RGB;
    } else if (depth == 4) {
        form = GL_RGBA;
    }

    int id_temp = texture_map[max_texture - 1].first;
    GLenum type_temp = texture_map[max_texture - 1].second;

    GLuint id;
    glCreateTextures(texture_type, 1, &id);
    if (id == 0) {
        delete []buf;
        SDL_FreeSurface(img_surface);
        ERRINFO("Create texture fail.");
        return id;
    }
    glActiveTexture(GL_TEXTURE0 + max_texture - 1);
    glBindTexture(texture_type, id);
    glTexImage2D(texture_type, 0, form, width, height, 0, form, GL_UNSIGNED_BYTE, buf);
    glGenerateMipmap(texture_type);
    glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, wrap_t);
    if (wrap_s == GL_CLAMP_TO_BORDER || wrap_t == GL_CLAMP_TO_BORDER) {
        float border_color[] = {r, g, b, a};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
    }
    glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, mag_filter);

    glBindTexture(type_temp, id_temp);

    delete []buf;
    SDL_FreeSurface(img_surface);

    return id;
}

bool Texture_manager::bind_texture(Shader &program, GLenum texture_type, const std::string &name, GLuint texture_id)
{
    int id = -1;
    for (int i = 0; i < max_texture; ++i) {
        if (texture_map[i].first == 0) {
            id = i;
            break;
        }
    }
    if (id == -1) {
        return false;
    }
    // why if the id is unsigned it will fail.
    if (!program.set_uniform(name, id)) {
        return false;
    }
    glActiveTexture(GL_TEXTURE0 + id);
    glBindTexture(texture_type, texture_id);
    texture_map[id] = std::make_pair(texture_id, texture_type);
    return true;
}

bool Texture_manager::unbind_texture(GLuint texture_id)
{
    for (int i = 0; i < max_texture; ++i) {
        if (texture_map[i].first == texture_id) {
            texture_map[i].first = 0;
            return true;
        }
    }
    return false;
}

void Texture_manager::destroy_texture(GLuint texture_id)
{
    if (texture_id == 0) {
        return;
    }
    unbind_texture(texture_id);
    glDeleteTextures(1, &texture_id);
}

void Texture_manager::set_texture_wrap(GLenum texture_type, GLuint texture_id, GLenum wrap_s, GLenum wrap_t, float r, float g, float b, float a)
{
    int id_temp = texture_map[max_texture - 1].first;
    GLenum type_temp = texture_map[max_texture - 1].second;

    glActiveTexture(GL_TEXTURE0 + max_texture - 1);
    glBindTexture(texture_type, texture_id);
    glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, wrap_t);
    if (wrap_s == GL_CLAMP_TO_BORDER || wrap_t == GL_CLAMP_TO_BORDER) {
        float border_color[] = {r, g, b, a};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
    }

    glBindTexture(type_temp, id_temp);
}

void Texture_manager::set_texture_filter(GLenum texture_type, GLuint texture_id, GLenum min_filter, GLenum mag_filter)
{
    int id_temp = texture_map[max_texture - 1].first;
    GLenum type_temp = texture_map[max_texture - 1].second;

    glActiveTexture(GL_TEXTURE0 + max_texture - 1);
    glBindTexture(texture_type, texture_id);
    glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, mag_filter);

    glBindTexture(type_temp, id_temp);
}

void Texture_manager::clean()
{
    for (int i = 0; i < max_texture; ++i) {
        texture_map[i].first = 0;
    }
}