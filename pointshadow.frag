#version 410 core

in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
	gl_FragDepth = length(FragPos.xyz - lightPos) / far_plane;
}