#pragma once

#include "Entity.h"
#include "Time.h"
#include "Maths.h"
#include "Terrain.h"

class Player : public Entity {

	int currentKey = 0;
	const float RUN_SPEED = 80;
	const float TURN_SPEED = 160;
	const float GRAVITY = -50;
	const float JUMP_POWER = 30;

	float currentSpeed = 0;
	float currentTurnSpeed = 0;
	float upwardSpeed = 0;

	bool isInAir = false;

public:

	Player(TexturedModel* model, glm::vec3 position, glm::vec3 rotation, float scale)
		: Entity(model, position, rotation, scale) {
	}

	void setKey(int key) {
		currentKey = key;

		if (currentKey == 1) {
			currentSpeed = RUN_SPEED;
		} else if (currentKey == 2) {
			currentSpeed = -RUN_SPEED;
		} else {
			currentSpeed = 0;
		}

		if (currentKey == 3) {
			currentTurnSpeed = TURN_SPEED;
		}
		else if (currentKey == 4) {
			currentTurnSpeed = -TURN_SPEED;
		}
		else {
			currentTurnSpeed = 0;
		}
	}

	void move(float deltaTime, Terrain& terrain) {
		float angle = currentTurnSpeed * deltaTime;
		this->increaseRotation(glm::vec3(0.0f, angle, 0.0f));
		float distance = currentSpeed * deltaTime;
		
		float deltaX = distance * sin(this->getRotation().y * ANGLE_TORADIANS_COF);
		float deltaZ = distance * cos(this->getRotation().y * ANGLE_TORADIANS_COF);

		this->increasePosition(glm::vec3(deltaX, 0.0f, deltaZ));

		float terrainH = terrain.getHeightOfTerrain(this->getPosition().x, this->getPosition().z);

		if (this->getPosition().y != terrainH) upwardSpeed += GRAVITY * deltaTime;
		this->increasePosition(glm::vec3(0.0f, upwardSpeed * deltaTime, 0.0f));
		if (this->getPosition().y < terrainH) {
			upwardSpeed = 0;
			this->getPosition() = glm::vec3(this->getPosition().x, terrainH, this->getPosition().z);
			this->updateTransformationMatrix();
			isInAir = false;
		}
	}

	void jump() {
		if (!isInAir) {
			isInAir = true;
			upwardSpeed += JUMP_POWER;
		}
	}
};