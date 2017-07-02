#pragma once

#include "ModelTexture.h"
#include "RawModel.h"
#include "Loader.h"
#include "TerrainTexturePack.h"
#include "thirdparty/lodepng.h"

class Terrain {
	const float SCALE = 2.0f;
	const float SIZE = 800;
	const float MAX_HEIGHT = 80;
	const float MAX_PIXEL_COLOUR = 256 /** 256 * 256*/;
	

	float x, z;

	RawModel* model;
	TerrainTexturePack* TexturePack;
	TerrainTexture* blendMap;

	float getHeight(int x, int z, int w, int h, std::vector<unsigned char>& image) {
		if (x < 0 || x >= w || z < 0 || z >= h) {
			return 0.0f;
		}

		glm::vec4 colour = { image[(x * w + z) * 4], image[(x * w + z) * 4 + 1],
							 image[(x * w + z) * 4 + 2], image[(x * w + z) * 4 + 3] };

		float height = (colour.r + colour.g + colour.b) / 3.0f;
		height /= MAX_PIXEL_COLOUR;
		height = height * 2.0f - 1.0f;

		height *= MAX_HEIGHT;

		return height;
	}

	glm::vec3 calculateNormal(int x, int z, int w, int h, std::vector<unsigned char>& image) {
		float heighL = getHeight(x - 1, z, w, h, image);
		float heighR = getHeight(x + 1, z, w, h, image);
		float heighD = getHeight(x, z - 1, w, h, image);
		float heighU = getHeight(x, z + 1, w, h, image);

		glm::vec3 normal = { heighL - heighR, 2.0f, heighD - heighU };
		normal = glm::normalize(normal);

		return normal;
	}

public:
	Terrain(int gridX, int gridZ, Loader& loader, TerrainTexturePack& TexturePack,
		    TerrainTexture& blendMap, std::string& heightMap) {
		this->TexturePack = &TexturePack;
		this->blendMap = &blendMap;
		x = gridX * SIZE * SCALE;
		z = gridZ * SIZE * SCALE;
		model = generateTerrain(loader, heightMap);
	}

	float getX() {
		return x;
	}

	float getZ() {
		return z;
	}

	RawModel* getModel() {
		return model;
	}

	TerrainTexturePack* getTexturePack() {
		return TexturePack;
	}

	TerrainTexture* getBlendMap() {
		return blendMap;
	}

	RawModel* generateTerrain(Loader& loader, std::string& heightMap) {
		std::vector<unsigned char> image;
		unsigned width, height;

		lodepng::decode(image, width, height, heightMap);

		int VERTEX_COUNT = width;

		int count = VERTEX_COUNT * VERTEX_COUNT;
		std::vector<float> vertices;
		vertices.resize(count * 3);

		std::vector<float> normals;
		normals.resize(count * 3);

		std::vector<float> textureCoords;
		textureCoords.resize(count * 2);

		std::vector<int> indices;
		indices.resize(6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1));

		int vertexPointer = 0;
		
		for (int i = 0; i < VERTEX_COUNT; i++) {
			for (int j = 0; j < VERTEX_COUNT; j++) {
				vertices[vertexPointer * 3] = (float)j / ((float)VERTEX_COUNT - 1) * SIZE;
				vertices[vertexPointer * 3 + 1] = getHeight(j, i, width, height, image);
				vertices[vertexPointer * 3 + 2] = (float)i / ((float)VERTEX_COUNT - 1) * SIZE;

				/*normals[vertexPointer * 3] = 0.0f;
				normals[vertexPointer * 3 + 1] = 0.0f;
				normals[vertexPointer * 3 + 2] = 0.0f;*/

				// "ThinMatrix" calculations
				//glm::vec3 normal = calculateNormal(j, i, width, height, image);

				//normals[vertexPointer * 3] = normal.x;
				//normals[vertexPointer * 3 + 1] = normal.y;
				//normals[vertexPointer * 3 + 2] = normal.z;

				textureCoords[vertexPointer * 2] = (float)j / ((float)VERTEX_COUNT - 1);
				textureCoords[vertexPointer * 2 + 1] = (float)i / ((float)VERTEX_COUNT - 1);
				vertexPointer++;
			}
		}

		int index = 0;
		vertexPointer = 0;
		for (int gz = 0; gz<VERTEX_COUNT - 1; gz++) {
			for (int gx = 0; gx<VERTEX_COUNT - 1; gx++) {
				int topLeft = (gz*VERTEX_COUNT) + gx;
				int topRight = topLeft + 1;
				int bottomLeft = ((gz + 1)*VERTEX_COUNT) + gx;
				int bottomRight = bottomLeft + 1;
				indices[index++] = topLeft;
				indices[index++] = bottomLeft;
				indices[index++] = topRight;
				indices[index++] = topRight;
				indices[index++] = bottomLeft;
				indices[index++] = bottomRight;
			}
		}

		// my normals calculations
		for (int i = 0; i < indices.size() - 1; i += 6) {
			glm::vec3 line0 = { vertices[indices[i + 1] * 3] - vertices[indices[i] * 3], vertices[indices[i + 1] * 3 + 1] - vertices[indices[i] * 3 + 1], vertices[indices[i + 1] * 3 + 2] - vertices[indices[i] * 3 + 2] };
			glm::vec3 line1 = { vertices[indices[i + 1] * 3] - vertices[indices[i + 2] * 3], vertices[indices[i + 1] * 3 + 1] - vertices[indices[i + 2] * 3 + 1], vertices[indices[i + 1] * 3 + 2] - vertices[indices[i + 2] * 3 + 2] };

			glm::vec3 normal0 = glm::cross(line1, line0);
			normal0 = glm::normalize(normal0);

			glm::vec3 line2 = { vertices[indices[i + 4] * 3] - vertices[indices[i + 2] * 3], vertices[indices[i + 4] * 3 + 1] - vertices[indices[i + 2] * 3 + 1], vertices[indices[i + 4] * 3 + 2] - vertices[indices[i + 2] * 3 + 2] };
			glm::vec3 line3 = { vertices[indices[i + 4] * 3] - vertices[indices[i + 5] * 3], vertices[indices[i + 4] * 3 + 1] - vertices[indices[i + 5] * 3 + 1], vertices[indices[i + 4] * 3 + 2] - vertices[indices[i + 5] * 3 + 2] };

			glm::vec3 normal1 = glm::cross(line3, line2);
			normal1 = glm::normalize(normal1);

			normals[indices[i] * 3] = normal0.x;
			normals[indices[i] * 3 + 1] = normal0.y;
			normals[indices[i] * 3 + 2] = normal0.z;

			normals[indices[i + 1] * 3] = normal0.x;
			normals[indices[i + 1] * 3 + 1] = normal0.y;
			normals[indices[i + 1] * 3 + 2] = normal0.z;

			normals[indices[i + 2] * 3] = normal0.x;
			normals[indices[i + 2] * 3 + 1] = normal0.y;
			normals[indices[i + 2] * 3 + 2] = normal0.z;

			normals[indices[i + 5] * 3] = normal1.x;
			normals[indices[i + 5] * 3 + 1] = normal1.y;
			normals[indices[i + 5] * 3 + 2] = normal1.z;
		}

		return loader.loadToVAO(vertices, textureCoords, normals, indices);
	}

	~Terrain() {
		delete TexturePack;
		delete blendMap;
	}

	
};