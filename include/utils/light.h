#ifndef LIGHT_H
#define LIGHT_H
#include "glm.hpp"
#include "shader.h"

struct LightColor {
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
};

class Light {
public:
    enum LightType {
        k_dirLight, k_pointLight, k_spotLight, k_globalLight
    };
    Light();
    LightType type() const;
    virtual bool applyToShader(Shader &shader, int index) const = 0;
    void setColor(const glm::vec4 &ambient = glm::vec4{0.0},
          const glm::vec4 &diffuse = glm::vec4{1.0},
          const glm::vec4 &specular = glm::vec4{1.0});
    virtual ~Light() = default;
protected:
    LightColor m_lc;
    LightType m_type;
    std::string m_name;
};

class Dirlight : public Light {
public:
    Dirlight(const glm::vec3 &dir);
    bool applyToShader(Shader &shader, int index) const override;
    void setDir(const glm::vec3 &dir);
protected:
    glm::vec3 m_dir;
};

class PointLight : public Light {
public:
    PointLight(const glm::vec3 &lightPos, float constant, float linear, float quadratic);
    void setPos(const glm::vec3 &lightPos);
    void setParamer(float constant, float linear, float quadratic);
    bool applyToShader(Shader &shader, int index) const override;
protected:
    glm::vec3 m_lPos;
    float m_constant, m_linear, m_quadratic;
};

class SpotLight : public PointLight {
public:
    SpotLight(const glm::vec3 &lightPos, const glm::vec3 &lightDir,
             float angleCos1, float angleCos2,
             float constant, float linear, float quadratic);
    void setParamer(float angleCos1, float angleCos2, float constant, float linear, float quadratic);
    void setDir(const glm::vec3 &dir);
    bool applyToShader(Shader &shader, int index) const override; 
protected:
    float m_angleCos1, m_angleCos2;
    glm::vec3 m_dir;
};

#endif