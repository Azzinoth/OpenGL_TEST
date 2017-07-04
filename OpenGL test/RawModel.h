#pragma once
#include "vector"

#include "thirdparty/glew2/include/GL/glew.h"
#include <GL/GL.h>

class RawModel {
	GLuint vaoID;
	int vertexCount;

public:
	RawModel(GLuint vaoID, int vertexCount) {
		this->vaoID = vaoID;
		this->vertexCount = vertexCount;
	}

	GLuint getVaoID() const {
		return vaoID;
	}

	int getVertexCount() const {
		return vertexCount;
	}
};