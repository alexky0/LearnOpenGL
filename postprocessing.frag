#version 410 core

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float offset_x;
uniform float offset_y;
uniform float gamma;

out vec4 FragColor;

vec2 offsets[9] = vec2[](
    vec2(-offset_x,  offset_y), vec2( 0.0f,  offset_y), vec2( offset_x,  offset_y),
    vec2(-offset_x,       0.0), vec2( 0.0f,       0.0), vec2( offset_x,       0.0),  
    vec2(-offset_x, -offset_y), vec2( 0.0f, -offset_y), vec2( offset_x, -offset_y) 
);

float kernel[9] = float[](
    0, 0, 0,
    0, 1, 0,
    0, 0, 0
);

void main()
{ 
    vec3 color = vec3(0.0);
    for(int i = 0; i < 9; i++)
        color += vec3(texture(screenTexture, TexCoords.st + offsets[i])) * kernel[i];
    FragColor = vec4(pow(color, vec3(1.0/gamma)), 1.0);
}