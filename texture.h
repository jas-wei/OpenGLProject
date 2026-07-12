#pragma once

#include <glad/glad.h> 
#include <glfw3.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Texture
{
	unsigned int textureID;

	public:
		Texture();

		void bind(int textureUnitOffset);
		void loadTexture(const char* texturePath, unsigned int TextureOption, int sourceFormat, int sourceDataType);
};

