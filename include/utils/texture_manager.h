#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H
#include <vector>
#include <string>
#include "glad/glad.h"

class Shader;
class Texture_manager {
public:
    static Texture_manager *instance();
    GLuint load_texture(GLenum texture_type, const std::string &image,
        GLenum min_filter = GL_NEAREST_MIPMAP_NEAREST, GLenum mag_filter = GL_LINEAR,
        GLenum wrap_s = GL_REPEAT, GLenum wrap_t = GL_REPEAT,
        float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 0.0f);
    bool reload_texture(GLuint id, GLenum texture_type, const std::string &image,
        GLenum min_filter = GL_NEAREST_MIPMAP_NEAREST, GLenum mag_filter = GL_LINEAR,
        GLenum wrap_s = GL_REPEAT, GLenum wrap_t = GL_REPEAT,
        float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 0.0f);
    GLuint load_cube_texture(const std::vector<std::string> &sky_img);
    bool reload_cube_texture(GLuint id, const std::vector<std::string> &sky_img);
    void set_texture_wrap(GLenum texture_type, GLuint texture_id, GLenum wrap_s, GLenum wrap_t, float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 0.0f);
    void set_texture_filter(GLenum texture_type, GLuint texture_id, GLenum min_filter, GLenum mag_filter);
    bool bind_texture(Shader &program, GLenum texture_type, const std::string &name, GLuint texture_id);
    bool unbind_texture(GLuint texture_id);
    void destroy_texture(GLuint texture_id);
    void clean();
    ~Texture_manager();
private:
    static Texture_manager inst;
    Texture_manager();
    const int max_texture;
    std::vector<std::pair<GLuint, GLenum>> texture_map;
};

#endif