#pragma once

#include "ShaderProgram.h"

static const char* const sGuiVertexShader = R"(
#version 140

in vec2 position;

out vec2 textureCoords;

uniform mat4 transformationMatrix;

void main(void){

	gl_Position = transformationMatrix * vec4(position, 0.0, 1.0);
	textureCoords = vec2((position.x+1.0)/2.0, 1 - (position.y+1.0)/2.0);
}
)";

static const char* const sGuiFragmentShader = R"(
#version 140

in vec2 textureCoords;

out vec4 out_Color;

uniform sampler2D guiTexture;

void main(void){

	out_Color = texture(guiTexture,textureCoords);

}
)";

class GuiShader : public ShaderProgram {
	GLuint location_transformationMatrix;

public:
	GuiShader() : ShaderProgram(sGuiVertexShader, sGuiFragmentShader,
								std::vector<std::string> { "position" }) {
		getAllUniformLocation();
	}

	void getAllUniformLocation() {
		location_transformationMatrix = getUniformLocation("transformationMatrix");
	}

	void loadTransformationMatrix(glm::mat4& matrix) {
		loadMatrix(location_transformationMatrix, matrix);
	}
};