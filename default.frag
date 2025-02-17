#version 410 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;

uniform Material material;

out vec4 FragColor;

void main()
{
   FragColor = texture(material.diffuse, TexCoords);
}