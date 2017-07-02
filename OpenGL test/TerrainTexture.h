#pragma once

#include "ModelTexture.h"

class TerrainTexture {
	GLuint textureID;
public:
	TerrainTexture(GLuint textureID) {
		this->textureID = textureID;
	}

	GLuint getTextureID() {
		return textureID;
	}
};