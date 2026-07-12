#include "texture.h"
#include <iostream>
#include "stb_image.h"

Texture::Texture() {
	glGenTextures(1, &textureID);
}

void Texture::bind(int textureUnitOffset) {
	glActiveTexture(GL_TEXTURE0 + textureUnitOffset);	// activate the texture unit first before binding texture
	glBindTexture(GL_TEXTURE_2D, textureID);			// place the texture at textureID into the currently active texture unit

}

void Texture::loadTexture(const char* texturePath, unsigned int TextureOption, int sourceFormat, int sourceDataType) {
	// Set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureOption);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureOption);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Loads image data into an array and populates ints tracking the image's width, height, and number of colour channels
	int width, height, nrChannels;
	unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);

	if (data) {
		// Binds image data to the current texture target
		// Params: Texture target, mip level, format, width, height, 0, source format, source datatype, image data
		glTexImage2D(GL_TEXTURE_2D, 0, sourceFormat, width, height, 0, sourceFormat, sourceDataType, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture data" << '\n';
	}

	stbi_image_free(data);
}