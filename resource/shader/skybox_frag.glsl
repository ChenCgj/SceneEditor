#version 450 core

in vec3 position;

out vec4 frag_color;

uniform samplerCube skybox;

void main()
{
    frag_color = texture(skybox, position);
}