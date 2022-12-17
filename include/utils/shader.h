#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H
#include <string>
#include <vector>
#include "glad/glad.h"

class Shader {
public:
    Shader();
    ~Shader();
    void setTransformFeedBackVaryings(std::vector<const char *> varyings, GLenum type);
    bool generate_program(const std::vector<std::string> &vert_src, const std::vector<std::string> &frag_src, const std::vector<std::string> &geometry_src = {});
    void delete_program();
    void use_program() const;
    template <typename T>
    bool set_uniform(const std::string &name, const T &value);
    void bindUniformBlock(const std::string &name, GLuint point);
    bool valid() const;
private:
    static bool compile_shader(GLuint shader);
    static GLuint link_program(GLuint vshader, GLuint fshader, GLuint gshader, const std::vector<const char *>varyings, GLenum varyingType);
    std::vector<const char *> varyings;
    GLuint program_id;
    GLenum varyingType;
};

#endif