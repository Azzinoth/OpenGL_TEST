#pragma once

#include "Camera.h"
#include "Entity.h"

class Entity;

class ModelViewCamera : public Camera {

	Entity* entity;

	float distanceFromEntity = 100;
	float angleAroundEntity = 0;

	int last_mouse_x = 0;
	int last_mouse_y = 0;

public:
	ModelViewCamera(Entity& entity) : Camera() {
		this->entity = &entity;
		pitch = 20;
	}

	virtual void setMouseWheel(int mouseWheel) final {
		if (mouseWheel > 0) {
			distanceFromEntity -= 4.0f;
		} else {
			distanceFromEntity += 4.0f;
		}
	}

	virtual void setMouseCoordinates(int mouseX, int mouseY) final {
		if (last_mouse_x == 0) last_mouse_x = mouseX;
		if (last_mouse_y == 0) last_mouse_y = mouseY;

		if (rightMouse) {
			float pitchChange = mouseY - last_mouse_y;
			pitch += pitchChange * 0.1f;
		}

		if (leftMouse) {
			float angleChange = mouseX - last_mouse_x;
			angleAroundEntity += angleChange * 0.1f;
		}

		last_mouse_y = mouseY;
		last_mouse_x = mouseX;
	}

	float calculateHorizontalDistance() {
		return distanceFromEntity * cos(pitch * glm::pi<float>() / 180.0f);
	}

	float calculateVerticalDistance() {
		return distanceFromEntity * sin(pitch * glm::pi<float>() / 180.0f);
	}

	void calculateCameraPosition(float horizDistance, float verticDistance) {
		float theta = entity->getRotation().y + angleAroundEntity;
		float offsetX = horizDistance * sin(theta * glm::pi<float>() / 180.0f);
		float offsetZ = horizDistance * cos(theta * glm::pi<float>() / 180.0f);

		position.x = entity->getPosition().x - offsetX;
		position.z = entity->getPosition().z - offsetZ;
		position.y = entity->getPosition().y + verticDistance;
	}

	virtual void move(float deltaTime) final {
		float horizontalDistance = calculateHorizontalDistance();
		float verticalDistance = calculateVerticalDistance();

		calculateCameraPosition(horizontalDistance, verticalDistance);
		yaw = 180 - (entity->getRotation().y + angleAroundEntity);
	}
};