#pragma once

#include "Shader.h"

using namespace std;

static const float rectangleVertices[] =
{
     1.0f, -1.0f,  1.0f, 0.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 1.0f,

     1.0f,  1.0f,  1.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 1.0f
};

class PostProcessing
{
public:
    Shader shader;
	unsigned int MSAA, textureMSAA, post, texturePost, RBO, VAO, VBO, width, height;
    PostProcessing(unsigned int width, unsigned int height, const char* vert, const char* frag, unsigned int samples);
    void Setup() const;
    void Draw() const;
};