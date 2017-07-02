#pragma once

#include "TexturedModel.h"
#include "Maths.h"

class Entity {
	TexturedModel* model;
	glm::vec3 position;
	glm::vec3 rotation;
	float scale;

	glm::mat4 transformationMatrix;
public:

	Entity(TexturedModel* model, glm::vec3 position, glm::vec3 rotation, float scale) {
		this->model = model;
		this->position = position;
		this->rotation = rotation;
		this->scale = scale;

		updateTransformationMatrix();
	}

	void increasePosition(glm::vec3 displacement) {
		position += displacement;
		updateTransformationMatrix();
	}

	void increaseRotation(glm::vec3 rotation) {
		this->rotation += rotation;
		updateTransformationMatrix();
	}

	TexturedModel* getModel() {
		return model;
	}

	void setModel(TexturedModel* model) {
		this->model = model;
	}

	glm::vec3& getPosition() {
		return position;
	}

	void setPosition(glm::vec3 position) {
		this->position = position;
		updateTransformationMatrix();
	}

	glm::vec3 getRotation() {
		return rotation;
	}

	void setRotation(glm::vec3 rotation) {
		this->rotation = rotation;
		updateTransformationMatrix();
	}

	float getScale() {
		return scale;
	}

	void setScale(float scale) {
		this->scale = scale;
		updateTransformationMatrix();
	}

	glm::mat4 getTransformationMatrix() {
		return transformationMatrix;
	}

	void setTransformationMatrix(glm::mat4 transformationMatrix) {
		this->transformationMatrix = transformationMatrix;
	}

	void updateTransformationMatrix() {
		transformationMatrix = createTransformationMatrix(position, rotation, scale);
	}
};