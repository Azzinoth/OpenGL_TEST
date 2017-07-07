#pragma once

#include "Maths.h"
#include "Terrain.h"

class MousePicker {

	const int RECURSION_COUNT = 300;
	const float RAY_RANGE = 10000;

	Terrain* terrain = nullptr;
	glm::vec3 currentTerrainPoint;

	glm::vec3 currentRay;

	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
	Camera* camera;

	int windowW, windowH;

	glm::vec3 getPointOnRay(glm::vec3 ray, float distance) {
		glm::vec3 camPos = camera->getPosition();
		glm::vec3 start = glm::vec3(camPos.x, camPos.y, camPos.z);
		glm::vec3 scaledRay = glm::vec3(ray.x * distance, ray.y * distance, ray.z * distance);

		return start + scaledRay;
	}

	glm::vec3 binarySearch(int count, float start, float finish, glm::vec3 ray) {
		float half = start + ((finish - start) / 2.0f);
		if (count >= RECURSION_COUNT) {
			glm::vec3 endPoint = getPointOnRay(ray, half);
			Terrain* terrain = getTerrain(endPoint.x, endPoint.z);
			if (terrain != nullptr) {
				return endPoint;
			}
			else {
				return glm::vec3(0.0f);
			}
		}
		if (intersectionInRange(start, half, ray)) {
			return binarySearch(count + 1, start, half, ray);
		}
		else {
			return binarySearch(count + 1, half, finish, ray);
		}
	}

	bool intersectionInRange(float start, float finish, glm::vec3 ray) {
		glm::vec3 startPoint = getPointOnRay(ray, start);
		glm::vec3 endPoint = getPointOnRay(ray, finish);
		if (!isUnderGround(startPoint) && isUnderGround(endPoint)) {
			return true;
		}
		else {
			return false;
		}
	}

	bool isUnderGround(glm::vec3 testPoint) {
		Terrain* terrain = getTerrain(testPoint.x, testPoint.z);
		float height = 0;
		if (terrain != nullptr) {
			height = terrain->getHeightOfTerrain(testPoint.x, testPoint.z);
		}
		if (testPoint.y < height) {
			return true;
		}
		else {
			return false;
		}
	}

	Terrain* getTerrain(float worldX, float worldZ) {
		return terrain;
	}

public:

	MousePicker(Camera& camera, glm::mat4 projectionMatrix, int windowW, int windowH, Terrain& terrain) {
		this->camera = &camera;
		this->projectionMatrix = projectionMatrix;
		viewMatrix = createViewMatrix(camera);
		this->windowW = windowW;
		this->windowH = windowH;
		this->terrain = &terrain;
	}

	glm::vec3 getCurrentTerrainPoint() {
		return currentTerrainPoint;
	}

	glm::vec3 getCurrentRay() {
		return currentRay;
	}

	void update() {
		viewMatrix = createViewMatrix(*camera);
		currentRay = calculateMouseRay();

		if (intersectionInRange(0, RAY_RANGE, currentRay)) {
			currentTerrainPoint = binarySearch(0, 0, RAY_RANGE, currentRay);
		}
		else {
			//currentTerrainPoint = nullptr;
		}
	}

	glm::vec2 getNormalizedDeviceCoords(float mouseX, float mouseY) {
		float x = (2.0f * mouseX) / windowW - 1;
		float y = (2.0f * mouseY) / windowH - 1;

		return glm::vec2(x, -y);
	}

	glm::vec4 toEyeCoords(glm::vec4 clipCoords) {
		glm::mat4 invertedProjection = glm::inverse(projectionMatrix);
		glm::vec4 eyeCoords = clipCoords * invertedProjection;

		return glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
	}

	glm::vec3 toWorldCoords(glm::vec4 eyeCoords) {
		glm::mat4 invertedView = glm::inverse(viewMatrix);
		glm::vec4 rayWorld = eyeCoords * invertedView;

		return glm::normalize(glm::vec3(rayWorld.x, rayWorld.y, rayWorld.z));
	}

	glm::vec3 calculateMouseRay() {
		float mouseX = Input::getInstance().getMouseX();
		float mouseY = Input::getInstance().getMouseY();
		
		glm::vec2 normalizedCoords = getNormalizedDeviceCoords(mouseX, mouseY);
		glm::vec4 clipCoords = glm::vec4(normalizedCoords.x, normalizedCoords.y, -1.0, 1.0);
		glm::vec4 eyeCoords = toEyeCoords(clipCoords);
		glm::vec3 worldRay = toWorldCoords(eyeCoords);

		return worldRay;
	}
};