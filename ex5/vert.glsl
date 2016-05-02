#version 150

in vec3 position;
in vec2 texcoord;
in vec3 color;

out vec3 Color;
out vec2 Texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    Texcoord = texcoord;
    Color = color;
    gl_Position = proj * view * model * vec4(position, 1.0);
}


