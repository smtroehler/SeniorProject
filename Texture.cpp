#include "Texture.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture() :
	filename(""),
	tid(0)
{
	
}

Texture::~Texture()
{
	
}

bool Texture::init()
{
   if (tid != 0)
      return true;
	// Load texture
	int w, h, ncomps;
   unsigned char *data = NULL;
      

   data = stbi_load(filename.c_str(), &w, &h, &ncomps, STBI_rgb_alpha);


	if(!data) {
		std::cerr << filename << " not found" << std::endl;
      return false;
	}
	if(ncomps < 3) {
		std::cerr << filename << " must have 3 components (RGB)" << std::endl;
      return false;
	}
	if((w & (w - 1)) != 0 || (h & (h - 1)) != 0) {
		std::cerr << filename << " must be a power of 2" << std::endl;
      return false;
	}
	width = w;
	height = h;
	
	// Generate a texture buffer object
	glGenTextures(1, &tid);
	// Bind the current texture to be the newly generated texture object
	glBindTexture(GL_TEXTURE_2D, tid);
	// Load the actual texture data
	// Base level is 0, number of channels is 3, and border is 0.
   assert(glGetError() == GL_NO_ERROR);
  
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	// Generate image pyramid
	glGenerateMipmap(GL_TEXTURE_2D);
   assert(glGetError() == GL_NO_ERROR);
	// Set texture wrap modes for the S and T directions
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set filtering mode for magnification and minimification
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// Unbind
	glBindTexture(GL_TEXTURE_2D, 0);
	// Free image, since the data is now on the GPU
	stbi_image_free(data);
   assert(glGetError() == GL_NO_ERROR);
   return true;
}

void Texture::bind(GLint handle, GLint unit)
{
   if (glGetError() != GL_NO_ERROR)
      std::cout << "error before but in Texture.bind\n";
	this->unit = unit;
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, tid);
	glUniform1i(handle, unit);
   if (glGetError() != GL_NO_ERROR)
      std::cout << "error in texture bind\n";
}

void Texture::unbind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, 0);
}
