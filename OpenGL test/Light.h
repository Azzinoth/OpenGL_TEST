#pragma once

#include "ShaderProgram.h"

class Light {
	glm::vec3 position;
	glm::vec3 colour;

public:
	Light(glm::vec3 position, glm::vec3 colour) {
		this->position = position;
		this->colour = colour;
	}

	glm::vec3 getPosition() {
		return position;
	}

	void setPosition(glm::vec3 position) {
		this->position = position;
	}

	glm::vec3 getColour() {
		return colour;
	}

	void setColour(glm::vec3 colour) {
		this->colour = colour;
	}
};