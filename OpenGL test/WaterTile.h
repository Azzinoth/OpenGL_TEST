#pragma once

class WaterTile {

	const float TILE_SIZE = 500; // 160

	float height;
	float x, z;

public:
	WaterTile(float centerX, float centerZ, float height) {
		this->x = centerX;
		this->z = centerZ;
		this->height = height;
	}

	float getHeight() {
		return height;
	}

	float getX() {
		return x;
	}

	float getZ() {
		return z;
	}

	float getTileSize() {
		return TILE_SIZE;
	}
};