#pragma once

#include "ShaderProgram.h"
#include "Light.h"

static const char* const sVertexShader = R"(
#version 400 core

in vec3 position;
in vec2 textureCoords;
in vec3 normal;

out vec2 pass_textureCoordinates;
out vec3 surfaceNormal;
out vec3 toLightVector[4];
out vec3 toCameraVector;
out float visibility;

uniform mat4 transformationMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform vec3 lightPosition[4];

uniform float useFakeLighting;

uniform float numberOfRows;
uniform vec2 offset;

const float density = 0.007;
const float gradient = 1.5;

void main(void) {
	vec4 worldPosition = transformationMatrix * vec4(position, 1.0);

	vec4 positionRelativeToCam = viewMatrix * worldPosition;
	gl_Position = projectionMatrix * positionRelativeToCam;
	pass_textureCoordinates = (textureCoords / numberOfRows) + offset; // offset

	vec3 actualNormal = normal;
	if (useFakeLighting > 0.5) {
		actualNormal = vec3(0.0, 1.0, 0.0);
	}

	surfaceNormal = (transformationMatrix * vec4(actualNormal, 0.0)).xyz;
	for (int i = 0; i < 4; i++) {
		//toLightVector[i] = lightPosition[i] - worldPosition.xyz;
	}

    toLightVector[0] = lightPosition[0] - worldPosition.xyz;
	toLightVector[1] = lightPosition[1] - worldPosition.xyz;
	toLightVector[2] = lightPosition[2] - worldPosition.xyz;
	toLightVector[3] = lightPosition[3] - worldPosition.xyz;

	toCameraVector = (inverse(viewMatrix) * vec4(0.0, 0.0, 0.0, 0.0)).xyz - worldPosition.xyz;

	float distance = length(positionRelativeToCam.xyz) / 14.0;
	//if (worldPosition.y < 5) distance = length(positionRelativeToCam.xyz) / 4.0; // fog on ground
	
	visibility = exp(-pow((distance * density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);
}
)";

static const char* const sFragmentShader = R"(
#version 400 core

in vec2 pass_textureCoordinates;
in vec3 surfaceNormal;
in vec3 toLightVector[4];
in vec3 toCameraVector;
in float visibility;

out vec4 out_Color;

uniform sampler2D modelTexture;
uniform vec3 lightColour[4];
uniform vec3 attenuation[4];
uniform float shineDamper;
uniform float reflectivity;
uniform vec3 skyColour;

void main(void) {

	vec3 unitNormal = normalize(surfaceNormal);
	vec3 unitVectorToCamera = normalize(toCameraVector);
	
	vec3 totalDiffuse = vec3(0.0);
	vec3 totalSpecular = vec3(0.0);

	for (int i = 0;i < 4; i++) {
		float distance = length(toLightVector[i]);
		float attFactor = attenuation[i].x + attenuation[i].y * distance + attenuation[i].z * distance * distance;
		vec3 unitLightVector = normalize(toLightVector[i]);
		float nDotl = dot(unitNormal, unitLightVector);
		float brightness = max(nDotl, 0.0);
		vec3 lightDirection = -unitLightVector;
		vec3 reflectedLightdirection = reflect(lightDirection, unitNormal);
		float specularFactor = dot(reflectedLightdirection, unitVectorToCamera);
		specularFactor = max(specularFactor, 0.0);
		float dampedFactor = pow(specularFactor, shineDamper);
		totalDiffuse = totalDiffuse + (brightness * lightColour[i]) / attFactor;
		totalSpecular = totalSpecular + (dampedFactor * reflectivity * lightColour[i]) / attFactor;
	}
	totalDiffuse = max(totalDiffuse, 0.2);

	vec4 textureColour = texture(modelTexture, pass_textureCoordinates);
	if (textureColour.a < 0.5) {
		discard;
	}
	
	out_Color = vec4(totalDiffuse, 1.0) * textureColour + vec4(totalSpecular, 1.0); // * (textureColour * 0.8)
	out_Color = mix (vec4(skyColour, 1.0), out_Color, visibility);

}
)";

class StaticShader : public ShaderProgram {

	const int MAX_LIGHTS = 4;

	GLuint location_transformationMatrix;
	GLuint location_projectionMatrix;
	GLuint location_viewMatrix;
	std::vector<GLuint> location_lightPosition;
	std::vector<GLuint> location_lightColour;
	std::vector<GLuint> location_attenuation;
	GLuint location_shineDamper;
	GLuint location_reflectivity;
	GLuint location_useFakeLighting;
	GLuint location_skyColour;
	GLuint location_numberOfRows;
	GLuint location_offset;
	
public:

	StaticShader() : 
		ShaderProgram(sVertexShader, sFragmentShader, 
		std::vector<std::string> {
		"position",
		"textureCoords",
		"normal"
	}) {
		getAllUniformLocation();
	};

	void getAllUniformLocation() {

		location_transformationMatrix = getUniformLocation("transformationMatrix");
		location_projectionMatrix = getUniformLocation("projectionMatrix");
		location_viewMatrix = getUniformLocation("viewMatrix");
		
		location_lightPosition.resize(MAX_LIGHTS);
		location_lightColour.resize(MAX_LIGHTS);
		location_attenuation.resize(MAX_LIGHTS);

		for (int i = 0; i < MAX_LIGHTS; i++) {
			std::string nameOfLocation = "lightPosition[";
			char buffer[10];
			_itoa(i, buffer, 10);
			nameOfLocation += buffer;
			nameOfLocation += "]";

			location_lightPosition[i] = getUniformLocation(nameOfLocation);

			nameOfLocation = "lightColour[";
			_itoa(i, buffer, 10);
			nameOfLocation += buffer;
			nameOfLocation += "]";

			location_lightColour[i] = getUniformLocation(nameOfLocation);

			nameOfLocation = "attenuation[";
			_itoa(i, buffer, 10);
			nameOfLocation += buffer;
			nameOfLocation += "]";

			location_attenuation[i] = getUniformLocation(nameOfLocation);
		}

		location_shineDamper = getUniformLocation("shineDamper");
		location_reflectivity = getUniformLocation("reflectivity");
		location_useFakeLighting = getUniformLocation("useFakeLighting");
		location_skyColour = getUniformLocation("skyColour");
		location_numberOfRows = getUniformLocation("numberOfRows");
		location_offset = getUniformLocation("offset");
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

	void loadLights(std::vector<Light*>& lights) {
		for (int i = 0; i < MAX_LIGHTS; i++) {
			if (i < lights.size()) {
				loadVector(location_lightPosition[i], lights[i]->getPosition());
				loadVector(location_lightColour[i], lights[i]->getColour());
				loadVector(location_attenuation[i], lights[i]->getAttenuation());
			} else {
				loadVector(location_lightPosition[i], glm::vec3(0.0, 0.0, 0.0));
				loadVector(location_lightColour[i], glm::vec3(0.0, 0.0, 0.0));
				loadVector(location_attenuation[i], glm::vec3(1.0, 0.0, 0.0));
			}
		}
	}

	void loadShineVariables(float damper, float reflectivity) {
		loadFloat(location_shineDamper, damper);
		loadFloat(location_reflectivity, reflectivity);
	}

	void loadFakeLightingVariable(bool useFake) {
		loadBool(location_useFakeLighting, useFake);
	}

	void loadSkyColour(glm::vec3 skyColour) {
		loadVector(location_skyColour, skyColour);
	}

	void loadNumberOfRows(int numberOfRows) {
		float temp = numberOfRows;
		loadFloat(location_numberOfRows, temp);
	}

	void loadOffset(float x, float y) {
		load2DVector(location_offset, glm::vec2(x, y));
	}

	~StaticShader() {
		cleanUp();
	}
};