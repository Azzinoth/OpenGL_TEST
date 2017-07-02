#include "thirdparty/glew2/include/GL/glew.h"
#include <GL/GL.h>
#include <windows.h>
#include <cstdio>
#include <iostream>
#include <cstdlib>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "PNGTextureLoader.h"

#include "thirdparty/lodepng.h"

GLuint png_texture_load(const char* file_name, int* width, int* height)
{
	std::vector<unsigned char> image; //the raw pixels
	unsigned width_, height_;

	//decode
	unsigned error = lodepng::decode(image, width_, height_, file_name);

	//if there's an error, display it
	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	//the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...

	// Generate the OpenGL texture object
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return texture;
}