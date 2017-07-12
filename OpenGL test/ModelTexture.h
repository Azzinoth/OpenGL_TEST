#pragma once

#include "thirdparty/glew2/include/GL/glew.h"
#include <GL/GL.h>

class ModelTexture {
	GLuint textureID;
	GLuint NormalMaptextureID;

	float shineDamper = 1;
	float reflectivity = 0;

	bool hasTransparency = false;
	bool useFakeLighting = false;

	int numberOfRows = 1;

public:
	ModelTexture(GLuint id) {
		this->textureID = id;
	}

	GLuint getID() {
		return textureID;
	}

	float getShineDamper() {
		return shineDamper;
	}

	void setShineDamper(float shineDamper) {
		this->shineDamper = shineDamper;
	}

	float getReflectivity() {
		return reflectivity;
	}

	void setReflectivity(float reflectivity) {
		this->reflectivity = reflectivity;
	}

	bool getHasTransparency() {
		return hasTransparency;
	}

	void setHasTransparency(float hasTransparency) {
		this->hasTransparency = hasTransparency;
	}

	bool getUseFakeLighting() {
		return useFakeLighting;
	}

	void setUseFakeLighting(float useFakeLighting) {
		this->useFakeLighting = useFakeLighting;
	}

	int getNumberOfRows() {
		return numberOfRows;
	}

	void setNumberOfRows(int numberOfRows) {
		this->numberOfRows = numberOfRows;
	}

	GLuint getNormalMaptextureID() {
		return NormalMaptextureID;
	}

	void setNormalMaptextureID(GLuint NormalMaptextureID) {
		this->NormalMaptextureID = NormalMaptextureID;
	}
};