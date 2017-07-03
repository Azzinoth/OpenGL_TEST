#pragma once

#include "thirdparty/glew2/include/GL/glew.h"
#include <GL/GL.h>
#include <vector>
#include "Camera.h"

#define ANGLE_TORADIANS_COF glm::pi<float>() / 180.0f

glm::mat4 createTransformationMatrix(glm::vec3& translation, glm::vec3& rotation, float scale) {
	glm::mat4 finalMatrix = glm::mat4();
	finalMatrix = glm::translate(finalMatrix, translation);

	finalMatrix = glm::rotate(finalMatrix, (float)rotation.x * ANGLE_TORADIANS_COF, glm::vec3(1, 0, 0));
	finalMatrix = glm::rotate(finalMatrix, (float)rotation.y * ANGLE_TORADIANS_COF, glm::vec3(0, 1, 0));
	finalMatrix = glm::rotate(finalMatrix, (float)rotation.z * ANGLE_TORADIANS_COF, glm::vec3(0, 0, 1));

	finalMatrix = glm::scale(finalMatrix, glm::vec3(scale, scale, scale));

	return finalMatrix;
}

glm::mat4 createViewMatrix(Camera& camera) {
	glm::mat4& ViewMatrix = camera.getViewMatrix();
	ViewMatrix = glm::mat4();

	ViewMatrix = glm::rotate(ViewMatrix, camera.getPitch() * ANGLE_TORADIANS_COF, glm::vec3(1, 0, 0));
	ViewMatrix = glm::rotate(ViewMatrix, camera.getYaw() * ANGLE_TORADIANS_COF, glm::vec3(0, 1, 0));
	ViewMatrix = glm::rotate(ViewMatrix, camera.getRoll() * ANGLE_TORADIANS_COF, glm::vec3(0, 0, 1));

	glm::vec3 cameraPosition = camera.getPosition();
	glm::vec3 negativeCameraPosition = -cameraPosition;

	ViewMatrix = glm::translate(ViewMatrix, negativeCameraPosition);

	return ViewMatrix;
}

float barryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos) {
	float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
	float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
	float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
	float l3 = 1.0f - l1 - l2;
	return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}
