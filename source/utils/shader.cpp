#include <sstream>
#include <fstream>
#include <string.h>
#include <iostream>
#include "glm.hpp"
#include "gtc/type_ptr.hpp"
#include "shader.h"
#include "debug.h"

template bool Shader::set_uniform<std::vector<int>>(const std::string &name, const std::vector<int> &value);
template bool Shader::set_uniform<std::vector<float>>(const std::string &name, const std::vector<float> &value);
template bool Shader::set_uniform<glm::vec3>(const std::string &name, const glm::vec3 &value);
template bool Shader::set_uniform<glm::vec4>(const std::string &name, const glm::vec4 &value);
template bool Shader::set_uniform<int>(const std::string &name, const int &value);
template bool Shader::set_uniform<unsigned int>(const std::string &name, const unsigned int &value);
template bool Shader::set_uniform<float>(const std::string &name, const float &value);
template bool Shader::set_uniform<glm::mat4>(const std::string &name, const glm::mat4 &value);

static void set_uniform(GLint location, const std::vector<int> &value);
static void set_uniform(GLint location, const std::vector<float> &value);
static void set_uniform(GLint location, const glm::vec3 &value);
static void set_uniform(GLint location, const glm::vec4 &value);
static void set_uniform(GLint location, int value);
static void set_uniform(GLint location, unsigned int value);
static void set_uniform(GLint location, float value);
static void set_uniform(GLint location, const glm::mat4 &value);

Shader::Shader() : program_id(0), varyingType{GL_NONE} {}

bool Shader::generate_program(const std::vector<std::string> &vert_src, const std::vector<std::string> &frag_src, const std::vector<std::string> &geometry_src)
{
    int nvsrc = vert_src.size(), nfsrc = frag_src.size(), ngsrc = geometry_src.size();
    GLuint vshader = 0, fshader = 0, gshader = 0;
    bool ret = false;
    bool has_geometry = false;

    typedef const char *PC;
    PC *vsrc = new PC[nvsrc];
    PC *fsrc = new PC[nfsrc];
    PC *gsrc = nullptr;
    if (ngsrc != 0 && geometry_src[0] != "") {
        has_geometry = true;
        gsrc = new PC[ngsrc];
    }
    if (vsrc == nullptr || fsrc == nullptr || (has_geometry && gsrc == nullptr)) {
        delete []vsrc;
        delete []fsrc;
        delete []gsrc;
        return false;
    }
    memset(vsrc, 0, sizeof(const char *) * nvsrc);
    memset(fsrc, 0, sizeof(const char *) * nfsrc);
    if (has_geometry) {
        memset(gsrc, 0, sizeof(const char *) * ngsrc);
    }

    std::stringstream ss;
    std::ifstream input;
    for (int i = 0; i < nvsrc; ++i) {
        input.open(vert_src[i], std::ios::in);
        if (!input) {
            ERRINFO("Open %s fail.", vert_src[i].c_str());
            goto errout;
        }
        ss.str("");
        ss << input.rdbuf();
        input.close();
        int len = ss.str().length();
        char *buf = new char[len + 1];
        if (buf == nullptr) {
            goto errout;
        }
        memcpy(buf, ss.str().c_str(), len + 1);
        vsrc[i] = buf;
    }
    for (int i = 0; i < nfsrc; ++i) {
        input.open(frag_src[i]);
        if (!input) {
            ERRINFO("Open %s fail.", frag_src[i].c_str());
            goto errout;
        }
        ss.str("");
        ss << input.rdbuf();
        input.close();
        int len = ss.str().length();
        char *buf = new char[len + 1];
        if (buf == nullptr) {
            goto errout;
        }
        memcpy(buf, ss.str().c_str(), len + 1);
        fsrc[i] = buf;
    }

    for (int i = 0; has_geometry && i < ngsrc; ++i) {
        input.open(geometry_src[i]);
        if (!input) {
            ERRINFO("Open %s fail.", geometry_src[i].c_str());
            goto errout;
        }
        ss.str("");
        ss << input.rdbuf();
        input.close();
        int len = ss.str().length();
        char *buf = new char[len + 1];
        if (buf == nullptr) {
            goto errout;
        }
        memcpy(buf, ss.str().c_str(), len + 1);
        gsrc[i] = buf;
    }
    vshader = glCreateShader(GL_VERTEX_SHADER);
    fshader = glCreateShader(GL_FRAGMENT_SHADER);
    if (has_geometry) {
        gshader = glCreateShader(GL_GEOMETRY_SHADER);
    }
    if (vshader == 0 || fshader == 0 || (has_geometry && gshader == 0)) {
        goto errout;
    }
    glShaderSource(vshader, nvsrc, vsrc, nullptr);
    glShaderSource(fshader, nfsrc, fsrc, nullptr);
    if (has_geometry) {
        glShaderSource(gshader, ngsrc, gsrc, nullptr);
    }
    if (!compile_shader(vshader) || !compile_shader(fshader) || (has_geometry && !compile_shader(gshader))) {
        goto errout;
    }
    program_id = link_program(vshader, fshader, gshader, varyings, varyingType);
    if (program_id != 0) {
        ret = true;
    }
errout:
    for (int i = 0; i < nvsrc; ++i) {
        delete []vsrc[i];
    }
    for (int i = 0; i < nfsrc; ++i) {
        delete []fsrc[i];
    }
    for (int i = 0; has_geometry && i < ngsrc; ++i) {
        delete []gsrc[i];
    }
    delete []vsrc;
    delete []fsrc;
    delete []gsrc;
    if (vshader != 0) {
        glDeleteShader(vshader);
    }
    if (fshader != 0) {
        glDeleteShader(fshader);
    }
    if (gshader != 0) {
        glDeleteShader(gshader);
    }
    return ret;
}

void Shader::delete_program()
{
    if (program_id == 0) {
        return;
    }
    glDeleteProgram(program_id);
    program_id = 0;
}

void Shader::use_program() const
{
    glUseProgram(program_id);
}

bool Shader::compile_shader(GLuint shader)
{
    glCompileShader(shader);
    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char buf[512];
        glGetShaderInfoLog(shader, sizeof(buf), nullptr, buf);
        ERRINFO("complile shader fail: %s", buf);
        return false;
    }
    return true;
}

GLuint Shader::link_program(GLuint vshader, GLuint fshader, GLuint gshader, const std::vector<const char *> varyings, GLenum varyingType)
{
    GLuint id = glCreateProgram();
    if (id == 0) {
        return id;
    }
    glAttachShader(id, vshader);
    glAttachShader(id, fshader);
    if (gshader != 0) {
        glAttachShader(id, gshader);
    }
    if (!varyings.empty()) {
        glTransformFeedbackVaryings(id, varyings.size(), &varyings[0], varyingType);
    }
    glLinkProgram(id);
    int success = 0;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        char buf[512];
        glGetProgramInfoLog(id, sizeof(buf), nullptr, buf);
        ERRINFO("link shader fail: %s", buf);
        glDeleteProgram(id);
        id = 0;
    }
    return id;
}

void Shader::bindUniformBlock(const std::string &name, GLuint point)
{
    GLuint index = glGetUniformBlockIndex(program_id, name.c_str());
    glUniformBlockBinding(program_id, index, point);
}

bool Shader::valid() const
{
    return program_id > 0;
}

template <typename T>
bool Shader::set_uniform(const std::string &name, const T &value)
{
    GLint location = glGetUniformLocation(program_id, name.c_str());
    if (location == -1) {
        return false;
    }
    use_program();
    ::set_uniform(location, value);
    return true;
}

void set_uniform(GLint location, const std::vector<float> &value)
{
    int size = value.size();
    if (size == 1) {
        glUniform1f(location, value[0]);
    } else if (size == 2) {
        glUniform2f(location, value[0], value[1]);
    } else if (size == 3) {
        glUniform3f(location, value[0], value[1], value[2]);
    } else if (size == 4) {
        glUniform4f(location, value[0], value[1], value[2], value[3]);
    }
}

void set_uniform(GLint location, const std::vector<int> &value)
{
    int size = value.size();
    if (size == 1) {
        glUniform1i(location, value[0]);
    } else if (size == 2) {
        glUniform2i(location, value[0], value[1]);
    } else if (size == 3) {
        glUniform3i(location, value[0], value[1], value[2]);
    } else if (size == 4) {
        glUniform4i(location, value[0], value[1], value[2], value[3]);
    }
}

void set_uniform(GLint location, const glm::vec3 &value)
{
    std::vector<float> v{value[0], value[1], value[2]};
    set_uniform(location, v);
}

void set_uniform(GLint location, const glm::vec4 &value)
{
    std::vector<float> v{value[0], value[1], value[2], value[3]};
    set_uniform(location, v);
}

void set_uniform(GLint location, int value)
{
    glUniform1i(location, value);
}

void set_uniform(GLint location, unsigned int value)
{
    glUniform1ui(location, value);
}

void set_uniform(GLint location, float value)
{
    glUniform1f(location, value);
}

void Shader::setTransformFeedBackVaryings(std::vector<const char *> varyings, GLenum type)
{
    this->varyings = varyings;
    this->varyingType = type;
}

void set_uniform(GLint location, const glm::mat4 &value)
{
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}