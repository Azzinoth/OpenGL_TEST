#pragma once
#include "vector"

#include "thirdparty/glew2/include/GL/glew.h"
#include <GL/GL.h>

#include "Maths.h"

class RawModel {
	GLuint vaoID = -1;
	glm::vec3 min = glm::vec3 (0.0);
	glm::vec3 max = glm::vec3(0.0);
	int vertexCount;

public:
	RawModel(GLuint vaoID, int vertexCount) {
		this->vaoID = vaoID;
		this->vertexCount = vertexCount;
	}

	GLuint getVaoID() const {
		return vaoID;
	}

	glm::vec3 getBoundingboxMin() const {
		return min;
	}

	void setBoundingboxMin(glm::vec3 min) {
		this->min = min;
	}

	glm::vec3 getBoundingboxMax() const {
		return max;
	}

	void setBoundingboxMax(glm::vec3 max) {
		this->max = max;
	}

	int getVertexCount() const {
		return vertexCount;
	}
};