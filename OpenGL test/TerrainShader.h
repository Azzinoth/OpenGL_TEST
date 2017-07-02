#pragma once

#include "ShaderProgram.h"
#include "Light.h"

static const char* const sTerrainVertexShader = R"(
#version 400 core

in vec3 position;
in vec2 textureCoords;
in vec3 normal;

out vec2 pass_texturecoords;
out vec3 surfaceNormal;
out vec3 toLightVector;
out vec3 toCameraVector;
out float visibility;

uniform mat4 transformationMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform vec3 lightPosition;

const float density = 0.007;
const float gradient = 1.5;

void main(void) {
	vec4 worldPosition = transformationMatrix * vec4(position, 1.0);

	vec4 positionRelativeToCam = viewMatrix * worldPosition;
	gl_Position = projectionMatrix * positionRelativeToCam;
	pass_texturecoords = textureCoords;

	surfaceNormal = (transformationMatrix * vec4(normal, 0.0)).xyz;
	toLightVector = lightPosition - worldPosition.xyz;
	toCameraVector = (inverse(viewMatrix) * vec4(0.0, 0.0, 0.0, 0.0)).xyz - worldPosition.xyz;

	float distance = length(positionRelativeToCam.xyz) / 14.0; // fog on ground set 2.0 or 7.0 to disable
	visibility = exp(-pow((distance * density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);
}
)";

static const char* const sTerrainFragmentShader = R"(
#version 400 core

in vec2 pass_texturecoords;
in vec3 surfaceNormal;
in vec3 toLightVector;
in vec3 toCameraVector;
in float visibility;

out vec4 out_Colour;

uniform sampler2D backgroundTexture;
uniform sampler2D rTexture;
uniform sampler2D gTexture;
uniform sampler2D bTexture;
uniform sampler2D blendMap;

uniform vec3 lightColour;
uniform float shineDamper;
uniform float reflectivity;
uniform vec3 skyColour;

void main(void) {

	vec4 blendMapColour = texture(blendMap, pass_texturecoords);
	
	float backTextureAmount = 1 - (blendMapColour.r + blendMapColour.g + blendMapColour.b);
	vec2 tiledCoords = pass_texturecoords * 40.0;
	vec4 backgroundTexturecolour = texture(backgroundTexture, tiledCoords) * backTextureAmount;
	vec4 rTexturecolour = texture(rTexture, tiledCoords) * blendMapColour.r;
	vec4 gTexturecolour = texture(gTexture, tiledCoords) * blendMapColour.g;
	vec4 bTexturecolour = texture(bTexture, tiledCoords) * blendMapColour.b;

	vec4 totalColour = backgroundTexturecolour + rTexturecolour + gTexturecolour + bTexturecolour;

	vec3 unitNormal = normalize(surfaceNormal);
	vec3 unitLightVector = normalize(toLightVector);

	float nDotl = dot(unitNormal, unitLightVector);
	float brightness = max(nDotl, 0.2); // Ambient light(don't correct);
	vec3 diffuse = brightness * lightColour;

	vec3 unitVectorToCamera = normalize(toCameraVector);
	vec3 lightDirection = -unitLightVector;
	vec3 reflectedLightdirection = reflect(lightDirection, unitNormal);
	
	float specularFactor = dot(reflectedLightdirection, unitVectorToCamera);
	specularFactor = max(specularFactor, 0.0);
	float dampedFactor = pow(specularFactor, shineDamper);
	vec3 finalSpecular = dampedFactor * reflectivity * lightColour;

	out_Colour = vec4(diffuse, 1.0) * totalColour + vec4(finalSpecular, 1.0);
	out_Colour = mix (vec4(skyColour, 1.0), out_Colour, visibility);

}
)";

class TerrainShader : public ShaderProgram {

	GLuint location_transformationMatrix;
	GLuint location_projectionMatrix;
	GLuint location_viewMatrix;
	GLuint location_lightPosition;
	GLuint location_lightColour;
	GLuint location_shineDamper;
	GLuint location_reflectivity;
	GLuint location_skyColour;
	GLuint location_backgroundTexture;
	GLuint location_rTexture;
	GLuint location_gTexture;
	GLuint location_bTexture;
	GLuint location_blendMap;

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
		location_lightPosition = getUniformLocation("lightPosition");
		location_lightColour = getUniformLocation("lightColour");
		location_shineDamper = getUniformLocation("shineDamper");
		location_reflectivity = getUniformLocation("reflectivity");
		location_skyColour = getUniformLocation("skyColour");
		location_backgroundTexture = getUniformLocation("backgroundTexture");
		location_rTexture = getUniformLocation("rTexture");
		location_gTexture = getUniformLocation("gTexture");
		location_bTexture = getUniformLocation("bTexture");
		location_blendMap = getUniformLocation("blendMap");
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

	void loadLight(Light& light) {
		loadVector(location_lightColour, light.getColour());
		loadVector(location_lightPosition, light.getPosition());
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

	~TerrainShader() {
		cleanUp();
	};
};