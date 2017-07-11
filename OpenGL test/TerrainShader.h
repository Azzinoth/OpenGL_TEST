#pragma once

#include "ShaderProgram.h"
#include "Light.h"

static const char* const sTerrainVertexShader = R"(
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

const float density = 0.007;
const float gradient = 1.5;

uniform vec4 plane;

void main(void) {
	vec4 worldPosition = transformationMatrix * vec4(position, 1.0);

	gl_ClipDistance[0] = dot(worldPosition, plane);

	vec4 positionRelativeToCam = viewMatrix * worldPosition;
	gl_Position = projectionMatrix * positionRelativeToCam;
	pass_textureCoordinates = textureCoords;

	surfaceNormal = (transformationMatrix * vec4(normal, 0.0)).xyz;
	for (int i = 0; i < 4; i++) {
		toLightVector[i] = lightPosition[i] - worldPosition.xyz;
	}

	toCameraVector = (inverse(viewMatrix) * vec4(0.0, 0.0, 0.0, 0.0)).xyz - worldPosition.xyz;

	float distance = length(positionRelativeToCam.xyz) / 14.0; // fog on ground set 2.0 or 7.0 to disable
	visibility = exp(-pow((distance * density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);
}
)";

static const char* const sTerrainFragmentShader = R"(
#version 400 core

in vec2 pass_textureCoordinates;
in vec3 surfaceNormal;
in vec3 toLightVector[4];
in vec3 toCameraVector;
in float visibility;

out vec4 out_Color;

uniform sampler2D backgroundTexture;
uniform sampler2D rTexture;
uniform sampler2D gTexture;
uniform sampler2D bTexture;
uniform sampler2D blendMap;

uniform vec3 lightColour[4];
uniform vec3 attenuation[4];
uniform float shineDamper;
uniform float reflectivity;
uniform vec3 skyColour;

void main(void) {

	vec4 blendMapColour = texture(blendMap, pass_textureCoordinates);
	
	float backTextureAmount = 1 - (blendMapColour.r + blendMapColour.g + blendMapColour.b);
	vec2 tiledCoords = pass_textureCoordinates * 40.0;
	vec4 backgroundTexturecolour = texture(backgroundTexture, tiledCoords) * backTextureAmount;
	vec4 rTexturecolour = texture(rTexture, tiledCoords) * blendMapColour.r;
	vec4 gTexturecolour = texture(gTexture, tiledCoords) * blendMapColour.g;
	vec4 bTexturecolour = texture(bTexture, tiledCoords) * blendMapColour.b;

	vec4 totalColour = backgroundTexturecolour + rTexturecolour + gTexturecolour + bTexturecolour;

    vec3 unitNormal = normalize(surfaceNormal);
	vec3 unitVectorToCamera = normalize(toCameraVector);

	vec3 totalDiffuse = vec3(0.0);
	vec3 totalSpecular = vec3(0.0);

	for (int i = 0; i < 4; i++) {
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

	out_Color = vec4(totalDiffuse, 1.0) * totalColour + vec4(totalSpecular, 1.0);
	out_Color = mix (vec4(skyColour, 1.0), out_Color, visibility);

}
)";

class TerrainShader : public ShaderProgram {

	const int MAX_LIGHTS = 4;

	GLuint location_transformationMatrix;
	GLuint location_projectionMatrix;
	GLuint location_viewMatrix;
	std::vector<GLuint> location_lightPosition;
	std::vector<GLuint> location_lightColour;
	std::vector<GLuint> location_attenuation;
	GLuint location_shineDamper;
	GLuint location_reflectivity;
	GLuint location_skyColour;
	GLuint location_backgroundTexture;
	GLuint location_rTexture;
	GLuint location_gTexture;
	GLuint location_bTexture;
	GLuint location_blendMap;
	GLuint location_plane;

public:

	TerrainShader() :
		ShaderProgram(sTerrainVertexShader, sTerrainFragmentShader,
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
		location_skyColour = getUniformLocation("skyColour");
		location_backgroundTexture = getUniformLocation("backgroundTexture");
		location_rTexture = getUniformLocation("rTexture");
		location_gTexture = getUniformLocation("gTexture");
		location_bTexture = getUniformLocation("bTexture");
		location_blendMap = getUniformLocation("blendMap");
		location_plane = getUniformLocation("plane");
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
			}
			else {
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

	void loadSkyColour(glm::vec3 skyColour) {
		loadVector(location_skyColour, skyColour);
	}

	void connectTextureUnits() {
		loadInt(location_backgroundTexture, GLint(0));
		loadInt(location_rTexture, GLint(1));
		loadInt(location_gTexture, GLint(2));
		loadInt(location_bTexture, GLint(3));
		loadInt(location_blendMap, GLint(4));
	}

	void loadclipPlane(glm::vec4 plane) {
		loadVector(location_plane, plane);
	}

	~TerrainShader() {
		cleanUp();
	}
};