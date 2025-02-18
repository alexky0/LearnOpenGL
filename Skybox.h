#pragma once

#include <stb/stb_image.h>
#include "Camera.h"

static const float skyboxVertices[] =
{
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f
};

static const unsigned int skyboxIndices[] =
{
	1, 2, 6,
	6, 5, 1,
	0, 4, 7,
	7, 3, 0,
	4, 5, 6,
	6, 7, 4,
	0, 3, 2,
	2, 1, 0,
	0, 1, 5,
	5, 4, 0,
	3, 7, 6,
	6, 2, 3
};

class Skybox
{
private:
	unsigned int VAO, VBO, EBO, texture;
	Shader shader;
public:
	Skybox(const char* dir, const char* vert, const char* frag);
	void Update(Camera camera, unsigned int height, unsigned int width) const;
};