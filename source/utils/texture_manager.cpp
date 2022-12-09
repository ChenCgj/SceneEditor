#include <iostream>
#include "SDL_image.h"
#include "shader.h"
#include "texture_manager.h"
#include "debug.h"
using std::string;
using std::vector;

static char *load_image_data(const string &image, int &width, int &height, int &pitch)
{
    SDL_Surface *img_surface = IMG_Load(image.c_str());
    if (img_surface == nullptr) {
        ERRINFO("Load image fail. SDL_image: %s", IMG_GetError());
        return nullptr;
    }
    width = img_surface->w;
    height = img_surface->h;
    pitch = img_surface->pitch;
    const char *data = static_cast<const char *>(img_surface->pixels);
    char *buf = new char[pitch * height];
    if (buf == nullptr) {
        ERRINFO("Load image fail: no enought memory");
        SDL_FreeSurface(img_surface);
        return nullptr;
    }
    for (int i = 0; i < height; ++i) {
        memcpy(buf + pitch * (height - i - 1), data + pitch * i, pitch);
    }
    SDL_FreeSurface(img_surface);
    return buf;
}

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

GLuint Texture_manager::load_texture(GLenum texture_type, const string &image, GLenum min_filter, GLenum mag_filter, GLenum wrap_s, GLenum wrap_t, float r, float g, float b, float a)
{
    // if (glCompressedTexImage2D())

    int width, height, pitch;
    auto buf = load_image_data(image, width, height, pitch);
    GLenum form, form_to;
    int depth = pitch / width;
    if (depth == 1) {
        form = GL_RED;
        form_to = GL_COMPRESSED_RED;
    } else if (depth == 3) {
        form = GL_RGB;
        form_to = GL_COMPRESSED_RGB;
    } else if (depth == 4) {
        form = GL_RGBA;
        form_to = GL_COMPRESSED_RGBA;
    }

    int id_temp = texture_map[max_texture - 1].first;
    GLenum type_temp = texture_map[max_texture - 1].second;

    GLuint id;
    glCreateTextures(texture_type, 1, &id);
    if (id == 0) {
        delete []buf;
        ERRINFO("Create texture fail.");
        return id;
    }
    glActiveTexture(GL_TEXTURE0 + max_texture - 1);
    glBindTexture(texture_type, id);
    glTexImage2D(texture_type, 0, form_to, width, height, 0, form, GL_UNSIGNED_BYTE, buf);
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

    return id;
}

GLuint Texture_manager::load_cube_texture(const std::vector<std::string> &sky_img)
{
    int id_temp = texture_map[max_texture - 1].first;
    GLenum type_temp = texture_map[max_texture - 1].second;

    GLuint id;
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &id);
    if (id == 0) {
        ERRINFO("Create texture fail.");
        return 0;
    }
    glActiveTexture(GL_TEXTURE0 + max_texture - 1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    for (vector<string>::size_type i = 0; i < sky_img.size(); ++i) {
        // auto *data = load_image_data(sky_img[i], width, height, pitch); // this will flip the image, but the cube image seems not need
        SDL_Surface *img = IMG_Load(sky_img[i].c_str());
        if (img == nullptr) {
            glBindTexture(type_temp, id_temp);
            glDeleteTextures(1, &id);
            ERRINFO("load image %s fail. SDL_Image: %s", sky_img[i].c_str(), IMG_GetError());
            return 0;
        }
        auto data = img->pixels;
        int width, height, pitch;
        width = img->w;
        height = img->h;
        pitch = img->pitch;
        GLenum form, form_to;
        int depth = pitch / width;
        if (depth == 1) {
            form = GL_RED;
            form_to = GL_COMPRESSED_RED;
        } else if (depth == 3) {
            form = GL_RGB;
            form_to = GL_COMPRESSED_RGB;
        } else if (depth == 4) {
            form = GL_RGBA;
            form_to = GL_COMPRESSED_RGBA;
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, form_to, width, height, 0, form, GL_UNSIGNED_BYTE, data);
        SDL_FreeSurface(img);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(type_temp, id_temp);
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
