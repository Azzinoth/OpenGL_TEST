#pragma once
#ifndef GLCHECKER_H
#define GLCHECKER_H

#include "thirdparty/glew2/include/GL/glew.h"
#include <GL/GL.h>
#include "assert.h"

//#define GL_ERROR(glCall)\
//   glCall;\
//   GLenum error = glGetError();\
//   if (error != 0) {\
//	   assert(0);\
//   }

#define GL_ERROR(glCall)                                                                           \
    do                                                                                             \
    {                                                                                              \
        glCall;                                                                                    \
        GLenum error = glGetError();															   \
																								   \
		if (error != 0) {																		   \
			assert(0);																			   \
		}																						   \
																								   \
    } while (0)

#endif GLCHECKER_H