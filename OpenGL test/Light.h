#pragma once

#include "ShaderProgram.h"

class Light {
	glm::vec3 position;
	glm::vec3 colour;
	glm::vec3 attenuation = { 1.0f, 0.0f, 0.0f };

public:
	Light(glm::vec3 position, glm::vec3 colour) {
		this->position = position;
		this->colour = colour;
	}

	Light(glm::vec3 position, glm::vec3 colour, glm::vec3 attenuation) {
		this->position = position;
		this->colour = colour;
		this->attenuation = attenuation;
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

	glm::vec3 getAttenuation() {
		return attenuation;
	}
};