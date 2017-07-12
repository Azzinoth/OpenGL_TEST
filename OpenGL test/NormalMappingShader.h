#pragma once

#include "ShaderProgram.h"
#include "Light.h"

static const char* const sNormalMapVertexShader = R"(
#version 400 core

in vec3 position;
in vec2 textureCoordinates;
in vec3 normal;
in vec3 tangent;

out vec2 pass_textureCoordinates;
out vec3 toLightVector[4];
out vec3 toCameraVector;
out float visibility;

uniform mat4 transformationMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform vec3 lightPosition[4];

uniform float numberOfRows;
uniform vec2 offset;

const float density = 0;
const float gradient = 5.0;

uniform vec4 plane;

void main(void){

	vec4 worldPosition = transformationMatrix * vec4(position,1.0);
	gl_ClipDistance[0] = dot(worldPosition, plane);
	mat4 modelViewMatrix = viewMatrix * transformationMatrix;
	vec4 positionRelativeToCam = modelViewMatrix * vec4(position,1.0);
	gl_Position = projectionMatrix * positionRelativeToCam;
	
	pass_textureCoordinates = (textureCoordinates/numberOfRows) + offset;
	
	//vec3 surfaceNormal = (modelViewMatrix * vec4(normal,0.0)).xyz;
	vec3 surfaceNormal = (transformationMatrix * vec4(normal, 0.0)).xyz;

	vec3 norm = normalize(surfaceNormal);
	vec3 tang = normalize((modelViewMatrix * vec4(tangent, 0.0)).xyz);
	vec3 bitang = normalize(cross(norm, tang));
	
	mat3 toTangentSpace = mat3(
		tang.x, bitang.x, norm.x,
		tang.y, bitang.y, norm.y,
		tang.z, bitang.z, norm.z
	);

	for(int i=0;i<4;i++){
		//toLightVector[i] = toTangentSpace * (lightPosition[i] - positionRelativeToCam.xyz);
		toLightVector[i] = toTangentSpace * (lightPosition[i] - worldPosition.xyz);
	}
	//toCameraVector = toTangentSpace * (-positionRelativeToCam.xyz);
	toCameraVector = toTangentSpace * ((inverse(viewMatrix) * vec4(0.0, 0.0, 0.0, 0.0)).xyz - worldPosition.xyz);
	
	float distance = length(positionRelativeToCam.xyz);
	visibility = exp(-pow((distance*density),gradient));
	visibility = clamp(visibility,0.0,1.0);
	
}

)";

static const char* const sNormalMapFragmentShader = R"(
#version 400 core

in vec2 pass_textureCoordinates;
in vec3 toLightVector[4];
in vec3 toCameraVector;
in float visibility;

out vec4 out_Color;

uniform sampler2D modelTexture;
uniform sampler2D normalMap;
uniform vec3 lightColour[4];
uniform vec3 attenuation[4];
uniform float shineDamper;
uniform float reflectivity;
uniform vec3 skyColour;

void main(void){

	vec4 normalMapValue = 2.0 * texture(normalMap, pass_textureCoordinates, -1.0) - 1.0;

	vec3 unitNormal = normalize(normalMapValue.rgb);
	vec3 unitVectorToCamera = normalize(toCameraVector);
	
	vec3 totalDiffuse = vec3(0.0);
	vec3 totalSpecular = vec3(0.0);
	
	for(int i=0;i<4;i++){
		float distance = length(toLightVector[i]);
		float attFactor = attenuation[i].x + (attenuation[i].y * distance) + (attenuation[i].z * distance * distance);
		vec3 unitLightVector = normalize(toLightVector[i]);	
		float nDotl = dot(unitNormal,unitLightVector);
		float brightness = max(nDotl,0.0);
		vec3 lightDirection = -unitLightVector;
		vec3 reflectedLightDirection = reflect(lightDirection,unitNormal);
		float specularFactor = dot(reflectedLightDirection , unitVectorToCamera);
		specularFactor = max(specularFactor,0.0);
		float dampedFactor = pow(specularFactor,shineDamper);
		totalDiffuse = totalDiffuse + (brightness * lightColour[i])/attFactor;
		totalSpecular = totalSpecular + (dampedFactor * reflectivity * lightColour[i])/attFactor;
	}
	totalDiffuse = max(totalDiffuse, 0.2);
	
	vec4 textureColour = texture(modelTexture,pass_textureCoordinates, -1.0);
	if(textureColour.a<0.5){
		discard;
	}

	out_Color =  vec4(totalDiffuse,1.0) * textureColour + vec4(totalSpecular,1.0);
	out_Color = mix(vec4(skyColour,1.0),out_Color, visibility);

}

)";

class NormalMappingShader : public ShaderProgram {

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
	GLuint location_plane;
	GLuint location_modelTexture;
	GLuint location_normalMap;

	glm::vec3 getEyeSpacePosition(Light& light, glm::mat4& viewMatrix) {
		glm::vec3 position = light.getPosition();
		glm::vec4 eyeSpacePos = { position.x, position.y, position.z, 1.0f };
		eyeSpacePos = viewMatrix * eyeSpacePos;
		//Matrix4f.transform(viewMatrix, eyeSpacePos, eyeSpacePos);
		return glm::vec3(eyeSpacePos);
	}


public:

	NormalMappingShader() :
		ShaderProgram(sNormalMapVertexShader, sNormalMapFragmentShader,
			std::vector<std::string> {
		"position",
		"textureCoords",
		"normal",
		"tangent"
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
		location_plane = getUniformLocation("plane");
		location_modelTexture = getUniformLocation("modelTexture");
		location_normalMap = getUniformLocation("normalMap");
	}

	void loadTransformationMatrix(glm::mat4& matrix) {
		loadMatrix(location_transformationMatrix, matrix);
	}

	void loadProjectionMatrix(glm::mat4& projection) {
		loadMatrix(location_projectionMatrix, projection);
	}

	void loadViewMatrix(glm::mat4 viewMatrix) {
		loadMatrix(location_viewMatrix, viewMatrix);
	}

	void loadLights(std::vector<Light*>& lights, glm::mat4 viewMatrix) {
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

	void loadclipPlane(glm::vec4 plane) {
		loadVector(location_plane, plane);
	}

	void connectTextureUnits() {
		loadInt(location_modelTexture, 0);
		loadInt(location_normalMap, 1);
	}

	~NormalMappingShader() {
		cleanUp();
	}
};