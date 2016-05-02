#version 150

in vec3 Color;
in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D texFox;
uniform sampler2D texCat;

void main()
{
    vec4 colFox = texture(texFox, Texcoord);
    vec4 colCat = texture(texCat, Texcoord);
    outColor = mix(colCat, colFox, 0.5);
}


