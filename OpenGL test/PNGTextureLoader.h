#pragma once

#include "thirdparty/glew2/include/GL/glew.h"
#include <GL/GL.h>

GLuint png_texture_load(const char * file_name, int * width, int * height);
