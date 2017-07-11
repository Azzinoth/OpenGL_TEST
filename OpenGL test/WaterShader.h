#pragma once

#include "ShaderProgram.h"

static const char* const sWaterVertexShader = R"(
#version 400 core

in vec2 position;

out vec2 textureCoords;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main(void) {

	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position.x, 0.0, position.y, 1.0);
	textureCoords = vec2(position.x/2.0 + 0.5, position.y/2.0 + 0.5);
 
}
)";

static const char* const sWaterFragmentShader = R"(
#version 400 core

in vec2 textureCoords;

out vec4 out_Color;

void main(void) {

	out_Color = vec4(0.0, 0.0, 1.0, 1.0);

}
)";

class WaterShader : public ShaderProgram {

	GLuint location_transformationMatrix;
	GLuint location_projectionMatrix;
	GLuint location_viewMatrix;

public:

	WaterShader() :
		ShaderProgram(sWaterVertexShader, sWaterFragmentShader,
			std::vector<std::string> { "position" }) {
		getAllUniformLocation();
	};

	void getAllUniformLocation() {
		location_transformationMatrix = getUniformLocation("modelMatrix");
		location_projectionMatrix = getUniformLocation("projectionMatrix");
		location_viewMatrix = getUniformLocation("viewMatrix");
	}

	void loadTransformationMatrix(glm::mat4& matrix) {
		loadMatrix(location_transformationMatrix, matrix);
	}

	void loadProjectionMatrix(glm::mat4& projection) {
		loadMatrix(location_projectionMatrix, projection);
	}

	void loadViewMatrix(Camera& camera) {
		loadMatrix(location_viewMatrix, createViewMatrix(camera));
	}

	~WaterShader() {
		cleanUp();
	}
};