#pragma once

#include "ShaderProgram.h"

static const char* const sWaterVertexShader = R"(
#version 400 core

in vec2 position;

out vec4 clipSpace;
out vec2 textureCoords;
out vec3 toCameraVector;
out vec3 fromLightVector;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform vec3 cameraPosition;
uniform vec3 lightPosition;

const float tiling = 4.0;

void main(void) {
	
	vec4 worldPosition = modelMatrix * vec4(position.x, 0.0, position.y, 1.0);
	toCameraVector = cameraPosition - worldPosition.xyz;
	fromLightVector = worldPosition.xyz - lightPosition;
 
	clipSpace = projectionMatrix * viewMatrix * worldPosition;
	gl_Position = clipSpace;
	textureCoords = vec2(position.x/2.0 + 0.5, position.y/2.0 + 0.5) * tiling;
}
)";

static const char* const sWaterFragmentShader = R"(
#version 400 core

in vec4 clipSpace;
in vec2 textureCoords;
in vec3 toCameraVector;
in vec3 fromLightVector;

out vec4 out_Color;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform vec3 lightColor;

uniform float moveFactor;

const float waveStrength = 0.02;
const float shineDamper = 100.0;
const float reflectivity = 0.6;

void main(void) {
	
	vec2 normalizedDiviceSpace = (clipSpace.xy / clipSpace.w) / 2.0 + 0.5;
	vec2 refractionTexcoords = vec2(normalizedDiviceSpace.x, normalizedDiviceSpace.y);
	vec2 reflectionTexcoords = vec2(normalizedDiviceSpace.x, -normalizedDiviceSpace.y);

	float near = 0.1;
	float far = 15000.0;
	float depth = texture(depthMap, refractionTexcoords).r;
	float floorDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));

	depth = gl_FragCoord.z;
	float waterDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
	float waterDepth = floorDistance - waterDistance;

	vec2 distortedTexCoords = texture(dudvMap, vec2(textureCoords.x + moveFactor, textureCoords.y)).rg*0.1;
	distortedTexCoords = textureCoords + vec2(distortedTexCoords.x, distortedTexCoords.y+moveFactor);
	vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength * clamp(waterDepth / 100.0, 0.0, 1.0);

	refractionTexcoords += totalDistortion;
	refractionTexcoords = clamp(refractionTexcoords, 0.001, 0.999);

	reflectionTexcoords += totalDistortion;
	reflectionTexcoords.x = clamp(reflectionTexcoords.x, 0.001, 0.999);
	reflectionTexcoords.y = clamp(reflectionTexcoords.y, -0.999, -0.001);

	vec4 reflectionColor = texture(reflectionTexture, reflectionTexcoords);
	vec4 refractionColor = texture(refractionTexture, refractionTexcoords);
	
	vec4 normalMapColor = texture(normalMap, distortedTexCoords);
	vec3 normal = vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b * 3.0, normalMapColor.g * 2.0 - 1.0);
	normal = normalize(normal);

	vec3 viewVector = normalize(toCameraVector);
	float refractiveFactor = dot(viewVector, normal);
	refractiveFactor = pow(refractiveFactor, 2.0);
	refractiveFactor = clamp(refractiveFactor, 0.0, 1.0);

	vec3 reflectedLight = reflect(normalize(fromLightVector), normal);
	float specular = max(dot(reflectedLight, viewVector), 0.0);
	specular = pow(specular, shineDamper);
	vec3 specularHighlights = lightColor * specular * reflectivity * clamp(waterDepth / 30.0, 0.0, 1.0);

	//refractionColor = mix(refractionColor, vec4(0.0, 0.5, 0.3, 1.0), clamp(waterDepth/60.0, 0.0, 1.0));

	out_Color = mix(reflectionColor, refractionColor, refractiveFactor);
	out_Color = mix(out_Color, vec4(0.0, 0.3, 0.5, 1.0), 0.2) + vec4(specularHighlights, 0.0);
	out_Color.a = clamp(waterDepth / 30.0, 0.0, 1.0);

}
)";

// old distortion shader code
//vec2 distortion1 = (texture(dudvMap, vec2(textureCoords.x + moveFactor, textureCoords.y)).rg * 2.0 - 1.0) * waveStrength;
//vec2 distortion2 = (texture(dudvMap, vec2(-textureCoords.x + moveFactor, textureCoords.y + moveFactor)).rg * 2.0 - 1.0) * waveStrength;
//vec2 totalDistortion = distortion1 + distortion2;

class WaterShader : public ShaderProgram {

	GLuint location_transformationMatrix;
	GLuint location_projectionMatrix;
	GLuint location_viewMatrix;
	GLuint location_reflectionTexture;
	GLuint location_refractionTexture;
	GLuint location_dudvMap;
	GLuint location_moveFactor;
	GLuint location_cameraPosition;
	GLuint location_normalMap;
	GLuint location_lightPosition;
	GLuint location_lightColor;
	GLuint location_depthMap;

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
		location_reflectionTexture = getUniformLocation("reflectionTexture");
		location_refractionTexture = getUniformLocation("refractionTexture");
		location_dudvMap = getUniformLocation("dudvMap");
		location_moveFactor = getUniformLocation("moveFactor");
		location_cameraPosition = getUniformLocation("cameraPosition");
		location_normalMap = getUniformLocation("normalMap");
		location_lightPosition = getUniformLocation("lightPosition");
		location_lightColor = getUniformLocation("lightColor");
		location_depthMap = getUniformLocation("depthMap");
	}

	void loadTransformationMatrix(glm::mat4& matrix) {
		loadMatrix(location_transformationMatrix, matrix);
	}

	void loadProjectionMatrix(glm::mat4& projection) {
		loadMatrix(location_projectionMatrix, projection);
	}

	void loadViewMatrix(Camera& camera) {
		loadMatrix(location_viewMatrix, createViewMatrix(camera));
		loadVector(location_cameraPosition, camera.getPosition());
	}

	void connectTextureUnits() {
		loadInt(location_reflectionTexture, 0);
		loadInt(location_refractionTexture, 1);
		loadInt(location_dudvMap, 2);
		loadInt(location_normalMap, 3);
		loadInt(location_depthMap, 4);
	}

	void loadMoveFactor(float factor) {
		loadFloat(location_moveFactor, factor);
	}

	void loadLight(Light* sun) {
		loadVector(location_lightPosition, sun->getPosition());
		loadVector(location_lightColor, sun->getColour());
	}

	~WaterShader() {
		cleanUp();
	}
};