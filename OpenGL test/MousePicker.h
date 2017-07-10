#pragma once

#include "Maths.h"
#include "Terrain.h"

class MousePicker {

	const int RECURSION_COUNT = 200;
	const float RAY_RANGE = 5000.0;

	Terrain* terrain = nullptr;
	glm::dvec3 currentTerrainPoint;

	glm::dvec3 currentRay;

	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
	Camera* camera;

	int windowW, windowH;

	glm::dvec3 getPointOnRay(glm::dvec3 ray, float distance) {
		glm::dvec3 camPos = camera->getPosition();
		glm::dvec3 start = glm::dvec3(camPos.x, camPos.y, camPos.z);
		glm::dvec3 scaledRay = glm::dvec3(ray.x * distance, ray.y * distance, ray.z * distance);

		return start + scaledRay;
	}

	glm::dvec3 binarySearch(int count, float start, float finish, glm::dvec3 ray) {
		float half = start + ((finish - start) / 2.0f);
		if (count >= RECURSION_COUNT) {
			glm::dvec3 endPoint = getPointOnRay(ray, half);
			Terrain* terrain = getTerrain(endPoint.x, endPoint.z);
			if (terrain != nullptr) {
				return endPoint;
			}
			else {
				return glm::dvec3(0.0f);
			}
		}
		if (intersectionInRange(start, half, ray)) {
			return binarySearch(count + 1, start, half, ray);
		}
		else {
			return binarySearch(count + 1, half, finish, ray);
		}
	}

	bool intersectionInRange(float start, float finish, glm::dvec3 ray) {
		glm::dvec3 startPoint = getPointOnRay(ray, start);
		glm::dvec3 endPoint = getPointOnRay(ray, finish);
		if (!isUnderGround(startPoint) && isUnderGround(endPoint)) {
			return true;
		}
		else {
			return false;
		}
	}

	bool isUnderGround(glm::dvec3 testPoint) {
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

	glm::dvec3 getCurrentTerrainPoint() {
		return currentTerrainPoint;
	}

	glm::dvec3 getCurrentRay() {
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

	glm::dvec2 getNormalizedDeviceCoords(float mouseX, float mouseY) {
		float x = (2.0f * mouseX) / windowW - 1;
		float y = (2.0f * mouseY) / windowH - 1;

		return glm::dvec2(x, -y);
	}

	glm::dvec4 toEyeCoords(glm::dvec4 clipCoords) {
		glm::dmat4 invertedProjection = glm::inverse(projectionMatrix);
		glm::dvec4 eyeCoords =  invertedProjection * clipCoords;

		return glm::dvec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
	}

	glm::dvec3 toWorldCoords(glm::dvec4 eyeCoords) {
		glm::dmat4 invertedView = glm::inverse(viewMatrix);
		glm::dvec4 rayWorld = invertedView * eyeCoords;

		return glm::normalize(glm::dvec3(rayWorld.x, rayWorld.y, rayWorld.z));
	}

	glm::dvec3 calculateMouseRay() {
		float mouseX = Input::getInstance().getMouseX();
		float mouseY = Input::getInstance().getMouseY();
		
		glm::dvec2 normalizedCoords = getNormalizedDeviceCoords(mouseX, mouseY);
		glm::dvec4 clipCoords = glm::dvec4(normalizedCoords.x, normalizedCoords.y, -1.0, 1.0);
		glm::dvec4 eyeCoords = toEyeCoords(clipCoords);
		glm::dvec3 worldRay = toWorldCoords(eyeCoords);

		return worldRay;
	}
};