#include "Texture.h"



Texture::Texture(const char* path)
{
	image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGBA);
	glGenTextures(1, &texture);

	//texture wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	//texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture() : Texture("images/wall.jpg") { }

Texture::~Texture()
{
}

void Texture::use() const
{
	glBindTexture(GL_TEXTURE_2D, texture);
}

void Texture::drop() const {//TODO unuse texture
	glBindTexture(GL_TEXTURE_2D, 0);
}
