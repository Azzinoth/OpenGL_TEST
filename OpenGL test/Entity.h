#pragma once

#include "TexturedModel.h"
#include "Maths.h"
#include "Loader.h"

class Entity {
	TexturedModel* model = nullptr;
	RawModel* boundingBox = nullptr;
	glm::vec3 position;
	glm::vec3 rotation;
	float scale;

	glm::mat4 transformationMatrix;

	int textureIndex = 0;

	void generateBoundingbox(Loader& loader) {
		if (model == nullptr) return;

		RawModel* rawModel = model->getRawModel();

		float minX = rawModel->getBoundingboxMin().x;
		float minY = rawModel->getBoundingboxMin().y;
		float minZ = rawModel->getBoundingboxMin().z;

		float maxX = rawModel->getBoundingboxMax().x;
		float maxY = rawModel->getBoundingboxMax().y;
		float maxZ = rawModel->getBoundingboxMax().z;

		std::vector<float> VERTICES = {
			minX, maxY, minZ, // 0
			minX, minY, minZ, // 1
			maxX, minY, minZ, // 2
			maxX, minY, minZ, // 2
			maxX, maxY, minZ, // 3
			minX, maxY, minZ, // 0

			minX, minY, maxZ, // 4
			minX, minY, minZ, // 1
			minX, maxY, minZ, // 0
			minX, maxY, minZ, // 0
			minX, maxY, maxZ, // 5
			minX, minY, maxZ, // 4

			maxX, minY, minZ, // 2
			maxX, minY, maxZ, // 6
			maxX, maxY, maxZ, // 7
			maxX, maxY, maxZ, // 7
			maxX, maxY, minZ, // 3
			maxX, minY, minZ, // 2

			minX, minY, maxZ, // 4
			minX, maxY, maxZ, // 5
			maxX, maxY, maxZ, // 7
			maxX, maxY, maxZ, // 7
			maxX, minY, maxZ,  // 6
			minX, minY, maxZ,  // 4

			minX, maxY, minZ,  // 0
			maxX, maxY, minZ,  // 3
			maxX, maxY, maxZ, // 7
			maxX, maxY, maxZ, // 7
			minX, maxY, maxZ,  // 5
			minX, maxY, minZ,  // 0

			minX, minY, minZ,  // 1
			minX, minY, maxZ,  // 4
			maxX, minY, minZ,  // 2
			maxX, minY, minZ,  // 2
			minX, minY, maxZ,  // 4
			maxX, minY, maxZ  // 6
		};

		boundingBox = loader.loadToVAO(VERTICES, 3);
	}

public:

	Entity(Loader& loader, TexturedModel* model, glm::vec3 position, glm::vec3 rotation, float scale) {
		this->model = model;
		this->position = position;
		this->rotation = rotation;
		this->scale = scale;

		updateTransformationMatrix();
		generateBoundingbox(loader);
	}

	Entity(Loader& loader, TexturedModel* model, int textureIndex, glm::vec3 position, glm::vec3 rotation, float scale) {
		this->model = model;
		this->position = position;
		this->rotation = rotation;
		this->scale = scale;
		this->textureIndex = textureIndex;

		updateTransformationMatrix();
		generateBoundingbox(loader);
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

	float getTextureXOffset() {
		int column = textureIndex % model->getTexture()->getNumberOfRows();
		float returnValue = (float)column / (float)model->getTexture()->getNumberOfRows();
		return returnValue;
	}

	float getTextureYOffset() {
		int row = textureIndex / model->getTexture()->getNumberOfRows();
		float returnValue = (float)row / (float)model->getTexture()->getNumberOfRows();
		return returnValue;
	}

	RawModel* getBoundingbox() {
		return boundingBox;
	}
};