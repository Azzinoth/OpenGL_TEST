#pragma once

#include "thirdparty/glm/vec3.hpp"
#include "thirdparty/glm/vec4.hpp"
#include "thirdparty/glm/mat4x4.hpp"
#include "thirdparty/glm/gtc/matrix_transform.hpp"
#include "thirdparty/glm/gtc/type_ptr.inl"

#include "Time.h"

class Camera {

protected:
	glm::mat4 viewMatrix = glm::mat4();

	glm::vec3 position;
	float pitch = 0;
	float yaw = 0;
	float roll;

	int currentKey = 0;
	bool leftMouse = false;
	bool rightMouse = false;

public:

	Camera() {
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		pitch = 0.0f;
		yaw = 0.0f;
		roll = 0.0f;
	}

	Camera(glm::vec3 position) {
		this->position = position;
		pitch = 0.0f;
		yaw = 0.0f;
		roll = 0.0f;
	}

	void setKey(int key) {
		currentKey = key;
	}

	virtual void move(float deltaTime) { }
	virtual void setMouseCoordinates(int mouseX, int mouseY, int mouseWheel) { }
	virtual void setMouseWheel(int mouseWheel) { }
	virtual void setMouseLeftButtonState(bool pressed) {
		leftMouse = pressed;
	}

	virtual void setMouseRightButtonState(bool pressed) {
		rightMouse = pressed;
	}

	glm::vec3 getPosition() {
		return position;
	}

	void setPosition(glm::vec3 position) {
		this->position = position;
	}

	float getPitch() {
		return pitch;
	}

	void setPitch(float pitch) {
		this->pitch = pitch;
	}

	float getYaw() {
		return yaw;
	}

	void setYaw(float yaw) {
		this->yaw = yaw;
	}

	float getRoll() {
		return roll;
	}

	void setRoll(float roll) {
		this->roll = roll;
	}

	glm::mat4& getViewMatrix() {
		return viewMatrix;
	}
};