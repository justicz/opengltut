#version 150

in vec3 position;
in float color;

out float Color;

void main()
{
    Color = color;
    gl_Position = vec4(position, 1.0);
}


