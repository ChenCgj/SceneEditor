#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec3 aTexCood;

out vec3 cood;
out vec3 normal;
out vec3 basecolor;
out vec3 fragPos;

uniform sampler2D texNorm1;
uniform bool normValid;     // texNorm1 valid?
uniform bool colorValid;    // color valid?

uniform mat4 mmodel;
uniform mat4 mview;
uniform mat4 mproj;

void main()
{
    gl_Position = mproj * mview * mmodel * vec4(aPos, 1.0f);
    fragPos = vec3(mmodel * vec4(aPos, 1.0f));
    cood = aTexCood;
    if (colorValid) {
        basecolor = aColor;
    } else {
        basecolor = vec3(1.0, 1.0, 1.0);
    }
    if (normValid) {
        normal = transpose(inverse(mat3(mmodel))) * vec3(texture(texNorm1, vec2(aTexCood)));
    } else {
        normal = transpose(inverse(mat3(mmodel))) * aNormal;
    }
}