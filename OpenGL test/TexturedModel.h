#pragma once

#include "RawModel.h"
#include "ModelTexture.h"

class TexturedModel {
	RawModel* rawModel;
	ModelTexture* texture;

public:

	TexturedModel(RawModel* model, ModelTexture* texture) {
		this->rawModel = model;
		this->texture = texture;
	}

	RawModel* getRawModel() {
		return rawModel;
	}

	ModelTexture* getTexture() {
		return texture;
	}
};