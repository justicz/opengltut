#version 150

in vec3 Color;
in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D texFox;
uniform sampler2D texCat;
uniform float Fade;
uniform vec3 reflectionMultiple;

void main()
{
    vec4 colFox = texture(texFox, Texcoord);
    vec4 colCat = texture(texCat, Texcoord);
    outColor = vec4(reflectionMultiple, 1.0) * vec4(Color, 1.0) * mix(colCat, colFox, Fade);
}


