#pragma once

#include "ShaderProgram.h"
#include "Light.h"

static const char* const sVertexShader = R"(
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

uniform float useFakeLighting;

const float density = 0.007;
const float gradient = 1.5;

void main(void) {
	vec4 worldPosition = transformationMatrix * vec4(position, 1.0);

	vec4 positionRelativeToCam = viewMatrix * worldPosition;
	gl_Position = projectionMatrix * positionRelativeToCam;
	pass_texturecoords = textureCoords;

	vec3 actualNormal = normal;
	if (useFakeLighting > 0.5) {
		actualNormal = vec3(0.0, 1.0, 0.0);
	}

	surfaceNormal = (transformationMatrix * vec4(actualNormal, 0.0)).xyz;
	toLightVector = lightPosition - worldPosition.xyz;
	toCameraVector = (inverse(viewMatrix) * vec4(0.0, 0.0, 0.0, 0.0)).xyz - worldPosition.xyz;

	float distance = length(positionRelativeToCam.xyz) / 14.0;
	//if (worldPosition.y < 5) distance = length(positionRelativeToCam.xyz) / 4.0; // fog on ground
	
	visibility = exp(-pow((distance * density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);
}
)";

static const char* const sFragmentShader = R"(
#version 400 core

in vec2 pass_texturecoords;
in vec3 surfaceNormal;
in vec3 toLightVector;
in vec3 toCameraVector;
in float visibility;

out vec4 out_Colour;

uniform sampler2D textureSampler;
uniform vec3 lightColour;
uniform float shineDamper;
uniform float reflectivity;
uniform vec3 skyColour;

void main(void) {

	vec3 unitNormal = normalize(surfaceNormal);
	vec3 unitLightVector = normalize(toLightVector);

	float nDotl = dot(unitNormal, unitLightVector);
	float brightness = max(nDotl, 0.3); // Ambient light(don't correct);
	vec3 diffuse = brightness * lightColour;

	vec3 unitVectorToCamera = normalize(toCameraVector);
	vec3 lightDirection = -unitLightVector;
	vec3 reflectedLightdirection = reflect(lightDirection, unitNormal);
	
	float specularFactor = dot(reflectedLightdirection, unitVectorToCamera);
	specularFactor = max(specularFactor, 0.0);
	float dampedFactor = pow(specularFactor, shineDamper);
	vec3 finalSpecular = dampedFactor * reflectivity * lightColour;

	vec4 textureColour = texture(textureSampler, pass_texturecoords);
	if (textureColour.a < 0.5) {
		discard;
	}

	out_Colour = vec4(diffuse, 1.0) * textureColour + vec4(finalSpecular, 1.0);
	out_Colour = mix (vec4(skyColour, 1.0), out_Colour, visibility);

}
)";

class StaticShader : public ShaderProgram {

	GLuint location_transformationMatrix;
	GLuint location_projectionMatrix;
	GLuint location_viewMatrix;
	GLuint location_lightPosition;
	GLuint location_lightColour;
	GLuint location_shineDamper;
	GLuint location_reflectivity;
	GLuint location_useFakeLighting;
	GLuint location_skyColour;
	
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
		location_lightPosition = getUniformLocation("lightPosition");
		location_lightColour = getUniformLocation("lightColour");
		location_shineDamper = getUniformLocation("shineDamper");
		location_reflectivity = getUniformLocation("reflectivity");
		location_useFakeLighting = getUniformLocation("useFakeLighting");
		location_skyColour = getUniformLocation("skyColour");
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

	void loadFakeLightingVariable(bool useFake) {
		loadBool(location_useFakeLighting, useFake);
	}

	void loadSkyColour(glm::vec3 skyColour) {
		loadVector(location_skyColour, skyColour);
	}

	~StaticShader() {
		cleanUp();
	};
};