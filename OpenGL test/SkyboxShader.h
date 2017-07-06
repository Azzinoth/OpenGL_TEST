#pragma once

#include "ShaderProgram.h"
#include "Camera.h"

static const char* const sSkyboxVertexShader = R"(
#version 400

in vec3 position;
out vec3 textureCoords;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main(void){
	
	gl_Position = projectionMatrix * viewMatrix * vec4(position, 1.0); 
	textureCoords = position;
	
}
)";

static const char* const sSkyboxFragmentShader = R"(
#version 400

in vec3 textureCoords;
out vec4 out_Color;

uniform samplerCube cubeMap;
uniform samplerCube cubeMap2;
uniform float blendFactor;
uniform vec3 fogColor;

const float lowerLimit = 500.0;
const float upperLimit = 1500.0;

void main(void){
    vec4 texture1 = texture(cubeMap, textureCoords);
	vec4 texture2 = texture(cubeMap2, textureCoords);

    vec4 finalColor = mix(texture1, texture2, blendFactor);
	float factor = (textureCoords.y - lowerLimit) / (upperLimit - lowerLimit);
	factor = clamp(factor, 0.0, 1.0);

	out_Color = mix(vec4(fogColor, 1.0), finalColor, factor);

	// test. and we can see that texture coordinates of cube map is in world space in TEXTURE_2D it is in object space.
	//float factor = 1.0;
	//if (textureCoords.y > 1500.9) factor = 0.0;
}
)";

class SkyboxShader : public ShaderProgram {

	GLuint location_projectionMatrix;
	GLuint location_viewMatrix;
	GLuint location_fogColor;
	GLuint location_cubeMap;
	GLuint location_cubeMap2;
	GLuint location_blendFactor;

	const float ROTATE_SPEED = 1.0f;
	float rotation = 0;

public:

	SkyboxShader() : ShaderProgram(sSkyboxVertexShader, sSkyboxFragmentShader,
		std::vector<std::string> { "position" }) {
		getAllUniformLocation();
	}

	void getAllUniformLocation() {
		location_projectionMatrix = getUniformLocation("projectionMatrix");
		location_viewMatrix = getUniformLocation("viewMatrix");
		location_fogColor = getUniformLocation("fogColor");
		location_blendFactor = getUniformLocation("blendFactor");
		location_cubeMap = getUniformLocation("cubeMap");
		location_cubeMap2 = getUniformLocation("cubeMap2");
	}

	void loadProjectionMatrix(glm::mat4& matrix) {
		loadMatrix(location_projectionMatrix, matrix);
	}

	void loadViewMatrix(Camera& camera) {
		glm::mat4 viewMatrix = createViewMatrixForSkybox(camera);
		rotation += ROTATE_SPEED * Time::getInstance().getTimePassedFromLastCallS();
		viewMatrix = glm::rotate(viewMatrix, rotation * ANGLE_TORADIANS_COF, glm::vec3(0, 1, 0));

		loadMatrix(location_viewMatrix, viewMatrix);
	}

	void loadFogColor(glm::vec3 fogColor) {
		loadVector(location_fogColor, fogColor);
	}

	void loadBlendFactor(float blend) {
		loadFloat(location_blendFactor, blend);
	}

	void connectTextureUnits() {
		loadInt(location_cubeMap, 0);
		loadInt(location_cubeMap2, 1);
	}
};