#version 450 core

layout(location = 0) in vec3 aPos;

out vec3 position;

uniform mat4 mmodel;
uniform mat4 mview;
uniform mat4 mproj;

void main()
{
    position = aPos;
    gl_Position = mproj * mview * mmodel * vec4(aPos, 1.0f);
    gl_Position = gl_Position.xyww;
}