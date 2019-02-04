#pragma once

#include <Windows.h>
#include "GL/glew.h"

#include "../../Utils/stb_image.h"
#include "../Rendering/GLAbstraction/RenderingResources.h"
#include <map>

class TextureManager
{
public:

    // Loads a texture from file or finds the already loaded texture
    static Texture* loadTexture(std::string fileName, TextureType type);

    static void unloadAllTextures();

private:
	/*
		initialises a texture and retruns the id
	*/
	static GLuint initTexture(unsigned char * data, int width, int height, unsigned format = GL_RGB, unsigned type = GL_UNSIGNED_BYTE);
	
    static std::map<std::string, Texture*> loadedTextures;
};
