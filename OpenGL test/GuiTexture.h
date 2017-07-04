#pragma once

#include "Maths.h"

class GuiTexture {
	GLuint textureID;
	glm::vec2 position;
	glm::vec2 scale;

public:
	GuiTexture(GLuint textureID, glm::vec2 position, glm::vec2 scale) {
		this->textureID = textureID;
		this->position = position;
		this->scale = scale;
	}

	GLuint getTexture() {
		return textureID;
	}

	glm::vec2 getPosition() {
		return position;
	}

	void setPosition(glm::vec2 position) {
		this->position = position;
	}

	glm::vec2 getScale() {
		return scale;
	}

	void setScale(glm::vec2 scale) {
		this->scale = scale;
	}
};