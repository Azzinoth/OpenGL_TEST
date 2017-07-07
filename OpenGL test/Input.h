#pragma once

#include "MasterRenderer.h"
class MasterRenderer;

class Input {
	friend MasterRenderer;
	Input() {};

	float mouseX, mouseY;

public:

	static Input& getInstance() {
		static Input input;
		return input;
	}

	void setMouseCoordinates(float MouseX, float MouseY) {
		getInstance().mouseX = MouseX;
		getInstance().mouseY = MouseY;
	}

	float getMouseX() {
		return getInstance().mouseX;
	}

	float getMouseY() {
		return getInstance().mouseY;
	}
};