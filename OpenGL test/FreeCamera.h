#pragma once

#include "Camera.h"

class FreeCamera : public Camera {

	int last_mouse_x = 0;
	int last_mouse_y = 0;

	float current_mouse_x_angle = 0;
	float current_mouse_y_angle = 0;

	const int correctionToSensitivity = 5;
	HWND hWnd;

public:

	FreeCamera(HWND hWnd) : Camera() {
		this->hWnd = hWnd;
	}

	FreeCamera(HWND hWnd, glm::vec3 position) : Camera(position) {
		this->hWnd = hWnd;
	}

	virtual void move(float deltaTime) final {
		if (currentKey == 1) {
			glm::vec4 forward = { 0.0f, 0.0f, -1.0f, 0.0f };
			forward = forward * viewMatrix;
			glm::normalize(forward);

			position.x += forward.x;
			position.y += forward.y;
			position.z += forward.z;
		}
		if (currentKey == 2) {
			glm::vec4 forward = { 0.0f, 0.0f, -1.0f, 0.0f };
			forward = forward * viewMatrix;
			glm::normalize(forward);

			position.x -= forward.x;
			position.y -= forward.y;
			position.z -= forward.z;
		}

		if (currentKey == 3) {
			glm::vec4 right = { 1.0f, 0.0f, 0.0f, 0.0f };
			right = right * viewMatrix;
			glm::normalize(right);

			position.x -= right.x;
			position.y -= right.y;
			position.z -= right.z;
		}
		if (currentKey == 4) {
			glm::vec4 right = { 1.0f, 0.0f, 0.0f, 0.0f };
			right = right * viewMatrix;
			glm::normalize(right);

			position.x += right.x;
			position.y += right.y;
			position.z += right.z;
		}
	}

	virtual void setMouseCoordinates(int mouseX, int mouseY) final {
		if (last_mouse_x == 0) last_mouse_x = mouseX;
		if (last_mouse_y == 0) last_mouse_y = mouseY;

		auto setCursorToCenter = [&]() {
			if (hWnd == GetActiveWindow()) {
				RECT rc, screen_wr;
				GetClientRect(hWnd, &rc);
				GetWindowRect(hWnd, &screen_wr);

				last_mouse_x = screen_wr.left + ((rc.right - rc.left) / 2);
				last_mouse_y = screen_wr.top + ((rc.bottom - rc.top) / 2);

				SetCursorPos(last_mouse_x, last_mouse_y);
			}
		};

		if (last_mouse_x < mouseX && abs(last_mouse_x - mouseX) > correctionToSensitivity) {
			current_mouse_x_angle += (mouseX - last_mouse_x) * 0.3f;
			setCursorToCenter();
		}
		else if (abs(last_mouse_x - mouseX) > correctionToSensitivity) {
			current_mouse_x_angle += (mouseX - last_mouse_x) * 0.3f;
			setCursorToCenter();
		}

		if (last_mouse_y < mouseY && abs(last_mouse_y - mouseY) > correctionToSensitivity) {
			current_mouse_y_angle += (mouseY - last_mouse_y) * 0.3f;
			setCursorToCenter();
		}
		else if (abs(last_mouse_y - mouseY) > correctionToSensitivity) {
			current_mouse_y_angle += (mouseY - last_mouse_y) * 0.3f;
			setCursorToCenter();
		}

		setYaw(current_mouse_x_angle);
		setPitch(current_mouse_y_angle);
	}
};