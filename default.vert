#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightProjection;

out DATA
{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	mat4 projection;
	vec4 FragPosLight;
} data_out;

void main()
{
    data_out.FragPos = vec3(model * vec4(aPos, 1.0));
    data_out.Normal = mat3(transpose(inverse(model))) * aNormal;  
    data_out.TexCoords = aTexCoords;
	data_out.projection = projection;
	data_out.FragPosLight = lightProjection * vec4(data_out.FragPos, 1.0);
    gl_Position = view * model * vec4(aPos, 1.0);
}