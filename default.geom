#version 410 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in DATA
{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	mat4 projection;
	vec4 FragPosLight;
} data_in[];

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 FragPosLight;

void main()
{
	FragPos = data_in[0].FragPos;
	Normal = data_in[0].Normal;
	TexCoords = data_in[0].TexCoords;
	FragPosLight = data_in[0].FragPosLight;
	gl_Position = data_in[0].projection * gl_in[0].gl_Position;
	EmitVertex();

	FragPos = data_in[1].FragPos;
	Normal = data_in[1].Normal;
	TexCoords = data_in[1].TexCoords;
	FragPosLight = data_in[1].FragPosLight;
	gl_Position = data_in[1].projection * gl_in[1].gl_Position;
	EmitVertex();

	FragPos = data_in[2].FragPos;
	Normal = data_in[2].Normal;
	TexCoords = data_in[2].TexCoords;
	FragPosLight = data_in[2].FragPosLight;
	gl_Position = data_in[2].projection * gl_in[2].gl_Position;
	EmitVertex();

	EndPrimitive();
}