#pragma once

#include "ShaderProgram.h"

static const char* const sBoundingboxVertexShader = R"(
#version 400 core

in vec3 position;

uniform mat4 transformationMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main(void) {
	gl_Position = projectionMatrix * viewMatrix * transformationMatrix * vec4(position, 1.0);
}
)";

static const char* const sBoundingboxFragmentShader = R"(
#version 400 core

out vec4 out_Color;

uniform vec3 Color;

void main(void) {

	out_Color = vec4(Color, 1.0);
	
}
)";

class BoundingboxShader : public ShaderProgram {

	GLuint location_transformationMatrix;
	GLuint location_projectionMatrix;
	GLuint location_viewMatrix;
	GLuint location_color;

public:

	BoundingboxShader() :
		ShaderProgram(sBoundingboxVertexShader, sBoundingboxFragmentShader,
			std::vector<std::string> { "position" }) {
		getAllUniformLocation();
	};

	void getAllUniformLocation() {
		location_transformationMatrix = getUniformLocation("transformationMatrix");
		location_projectionMatrix = getUniformLocation("projectionMatrix");
		location_viewMatrix = getUniformLocation("viewMatrix");
		location_color = getUniformLocation("Color");
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

	void loadColor(glm::vec3 Color) {
		loadVector(location_color, Color);
	}

	~BoundingboxShader() {
		cleanUp();
	}
};