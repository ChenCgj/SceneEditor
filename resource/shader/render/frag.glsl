#version 450 core

struct MaterialColor {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

struct Light {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

struct DirLight {
    vec3 dir;
    Light light;
};

struct PointLight {
    vec3 lightPos;
    Light light;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 lightPos;
    vec3 lightDir;
    Light light;
    float angleCos1;
    float angleCos2;
    float constant;
    float linear;
    float quadratic;
};

vec4 calcDirLight(DirLight light, MaterialColor color, vec3 norm, vec3 view_dir);
vec4 calcPointLight(PointLight light, MaterialColor color, vec3 norm, vec3 view_dir, vec3 frag_pos);
vec4 calcSpotLight(SpotLight light, MaterialColor color, vec3 norm, vec3 view_dir, vec3 frag_pos);

// ---------------------------------------------------------------------------------------------------

#define MAX_POINT_LIGHT_COUNT 10
#define MAX_DIR_LIGHT_COUNT   10
#define MAX_SPOT_LIGHT_COUNT  10

in vec3 fragPos;
in vec3 normal;
in vec3 cood;
in vec3 basecolor;

uniform sampler2D texDiff1;
uniform sampler2D texSpec1;
// uniform sampler2D texNorm1; // in vertex shader
uniform sampler2D texHeig1;
uniform bool diffValid, specValid, heigValid;

uniform vec3 viewPos;
uniform DirLight dirLight[MAX_DIR_LIGHT_COUNT];
uniform uint dirLightCount;
uniform PointLight pointLight[MAX_POINT_LIGHT_COUNT];
uniform uint pointLightCount;
uniform SpotLight spotLight[MAX_SPOT_LIGHT_COUNT];
uniform uint spotLightCount;

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
    sampler2D tex1;
    sampler2D tex2;
};
uniform Material material;

out vec4 outColor;

void main()
{
    vec4 outColorTemp = vec4(0.0f);
    MaterialColor color;
    color.ambient = color.diffuse = vec4(basecolor, 1.0f);
    color.specular = vec4(0);
    if (diffValid) {
        color.ambient = color.diffuse = texture(texDiff1, vec2(cood));
    }
    if (specValid) {
        color.specular = texture(texSpec1, vec2(cood));
    }
    color.shininess = material.shininess;

    // vec3 c = vec3(0);
    for (int i = 0; i < dirLightCount; ++i) {
        outColorTemp += calcDirLight(dirLight[i], color, normal, viewPos - fragPos);
        // c += vec3(0.5);
    }
    for (int i = 0; i < pointLightCount; ++i) {
        outColorTemp += calcPointLight(pointLight[i], color, normal, viewPos - fragPos, fragPos);
    }
    for (int i = 0; i < spotLightCount; ++i) {
        outColorTemp += calcSpotLight(spotLight[i], color, normal, viewPos - fragPos, fragPos);
    }
    outColor = vec4(outColorTemp.rgb, 1.0);
    // outColor -= outColor;
    // outColor += vec4(c, 1);
    // outColor += texture(texDiff1, vec2(cood));
}

vec4 calcDirLight(DirLight light, MaterialColor color, vec3 norm, vec3 view_dir)
{
    vec4 ambient = light.light.ambient * color.ambient;

    vec3 dir = normalize(light.dir);
    vec3 normal = normalize(norm);
    float temp = max(0.0f, dot(-dir, normal));
    vec4 diffuse = light.light.diffuse * temp * color.diffuse;

    temp = pow(max(0.0f, dot(reflect(dir, normal), normalize(view_dir))), color.shininess);
    vec4 specular = light.light.specular * temp * color.specular;
    return ambient + diffuse + specular;
}

vec4 calcPointLight(PointLight light, MaterialColor color, vec3 norm, vec3 view_dir, vec3 frag_pos)
{
    vec4 ambient = light.light.ambient * color.ambient;

    vec3 dir_temp = frag_pos - light.lightPos;
    float distance = length(dir_temp);
    vec3 dir = normalize(dir_temp);

    vec3 normal = normalize(norm);
    float temp = max(0.0f, dot(-dir, normal));
    vec4 diffuse = light.light.diffuse * temp * color.diffuse;

    temp = pow(max(0.0f, dot(reflect(dir, normal), normalize(view_dir))), color.shininess);
    vec4 specular = light.light.specular * temp * color.specular;

    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * pow(distance, 2));
    return ambient + (diffuse + specular) * attenuation;
}

vec4 calcSpotLight(SpotLight light, MaterialColor color, vec3 norm, vec3 view_dir, vec3 frag_pos)
{
    vec4 ambient = light.light.ambient * color.ambient;

    vec3 dir_temp = frag_pos - light.lightPos;
    float distance = length(dir_temp);
    vec3 dir = normalize(dir_temp);

    vec3 normal = normalize(norm);
    float temp = max(0.0f, dot(-dir, normal));
    vec4 diffuse = light.light.diffuse * temp * color.diffuse;

    temp = pow(max(0.0f, dot(reflect(dir, normal), normalize(view_dir))), color.shininess);
    vec4 specular = light.light.specular * temp * color.specular;

    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * pow(distance, 2));
    attenuation *= clamp((dot(dir, normalize(light.lightDir)) - light.angleCos2) / (light.angleCos1 - light.angleCos2), 0.0f, 1.0f);

    return ambient + (diffuse + specular) * attenuation;
}