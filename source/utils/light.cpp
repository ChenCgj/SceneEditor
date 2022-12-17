#include "light.h"
#include "debug.h"

using glm::vec4;
using std::string;
using std::to_string;

static bool setCommonLight(const string &prefix, Shader &shader, const glm::vec4 &a, const glm::vec4 &d, const glm::vec4 &s);

Light::Light() : m_lc{vec4{0.0}, vec4{1.0}, vec4{1.0}}, m_type{k_pointLight}, m_name{"unknow"} {}

Light::LightType Light::type() const
{
    return m_type;
}

void Light::setColor(const glm::vec4 &a, const glm::vec4 &d, const glm::vec4 &s)
{
    m_lc.ambient = a;
    m_lc.diffuse = d;
    m_lc.specular = s;
}

Dirlight::Dirlight(const glm::vec3 &dir) : Light(), m_dir{glm::vec3(1.0)}
{
    m_name = "dirLight";
}

void Dirlight::setDir(const glm::vec3 &dir)
{
    m_dir = dir;
}

bool Dirlight::applyToShader(Shader &shader, int index) const
{
    string prefix = m_name + "[" + to_string(index) + "].";
    if (!setCommonLight(prefix, shader, m_lc.ambient, m_lc.diffuse, m_lc.specular)) {
        ERRINFO("Set common light attrib fail.");
        return false;
    }
    if (!shader.set_uniform(prefix + "dir", m_dir)) {
        ERRINFO("Set dir light dir fail.");
        return false;
    }
    return true;
}

bool setCommonLight(const string &prefix, Shader &shader, const glm::vec4 &a, const glm::vec4 &d, const glm::vec4 &s)
{
    bool ret = true;
    ret = ret && shader.set_uniform(prefix + "light.ambient", a);
    ret = ret && shader.set_uniform(prefix + "light.diffuse", d);
    ret = ret && shader.set_uniform(prefix + "light.specular", s);
    return ret;
}

PointLight::PointLight(const glm::vec3 &lightPos, float constant, float linear, float quadratic) :
    m_lPos(lightPos), m_constant(constant), m_linear(linear), m_quadratic(quadratic)
{
    m_name = "pointLight";
}

void PointLight::setPos(const glm::vec3 &lightPos)
{
    m_lPos = lightPos;
}

void PointLight::setParamer(float constant, float linear, float quadratic)
{
    m_constant = constant;
    m_linear = linear;
    m_quadratic = quadratic;
}

bool PointLight::applyToShader(Shader &shader, int index) const
{
    string prefix = m_name + "[" + to_string(index) +"].";
    if (!setCommonLight(prefix, shader, m_lc.ambient, m_lc.diffuse, m_lc.specular)) {
        ERRINFO("Set common light attrib fail.");
        return false;
    }
    if (!shader.set_uniform(prefix + "lightPos", m_lPos) ||
        !shader.set_uniform(prefix + "constant", m_constant) ||
        !shader.set_uniform(prefix + "linear", m_linear) ||
        !shader.set_uniform(prefix + "quadratic", m_quadratic)) {

        ERRINFO("Set point light attribute fail.");
        return false;
    }
    return true;
}

SpotLight::SpotLight(const glm::vec3 &lightPos, const glm::vec3 &lightDir, float angleCos1, float angleCos2, float constant, float linear, float quadratic) :
                     PointLight(lightPos, constant, linear, quadratic), m_angleCos1(angleCos1), m_angleCos2(angleCos2), m_dir(lightDir)
{
    m_name = "spotLight";
}

void SpotLight::setParamer(float angleCos1, float angleCos2, float constant, float linear, float quadratic)
{
    m_angleCos1 = angleCos1;
    m_angleCos2 = angleCos2;
    PointLight::setParamer(constant, linear, quadratic);
}

void SpotLight::setDir(const glm::vec3 &dir)
{
    m_dir = dir;
}

bool SpotLight::applyToShader(Shader &shader, int index) const
{
    string prefix = m_name + "[" + to_string(index) + "].";
    if (!setCommonLight(prefix, shader, m_lc.ambient, m_lc.diffuse, m_lc.specular)) {
        ERRINFO("Set common light attrib fail.");
        return false;
    }
    if (!shader.set_uniform(prefix + "lightPos", m_lPos) ||
        !shader.set_uniform(prefix + "constant", m_constant) ||
        !shader.set_uniform(prefix + "linear", m_linear) ||
        !shader.set_uniform(prefix + "quadratic", m_quadratic) ||
        !shader.set_uniform(prefix + "lightDir", m_dir) ||
        !shader.set_uniform(prefix + "angleCos1", m_angleCos1) ||
        !shader.set_uniform(prefix + "angleCos2", m_angleCos2)) {

        ERRINFO("Set spot light attribute fail.");
        return false;
    }
    return true;
}