#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTexCood;

out vec2 cood;
out vec3 normal;
out vec3 basecolor;
out vec3 fragPos;
uniform mat4 mmodel;
uniform mat4 mview;
uniform mat4 mproj;

void main()
{
    gl_Position = mproj * mview * mmodel * vec4(aPos, 1.0f);
    cood = aTexCood;
    basecolor = aColor;
    fragPos = vec3(mmodel * vec4(aPos, 1.0f));
    normal = transpose(inverse(mat3(mmodel))) * aNormal;
}