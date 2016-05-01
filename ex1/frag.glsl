#version 150

in float Color;

out vec4 outColor;

void main()
{
    outColor = vec4(Color, Color, Color, 1.0);
}


