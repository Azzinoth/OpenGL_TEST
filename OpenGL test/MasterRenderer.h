#pragma once

#include "EntityRenderer.h"
#include "TerrainRenderer.h"
#include "Time.h"
class Time;

#include <map>

class MasterRenderer {

	Time* time;

	StaticShader* shader;
	EntityRenderer* renderer;

	TerrainRenderer* terrainRenderer;
	TerrainShader* terrainShader;

	std::map<TexturedModel*, std::vector<Entity*>> entities;
	std::vector<Terrain*> terrains;

	const float FOV = 70.0f;
	const float NEAR_PLANE = 0.1f;
	const float FAR_PLANE = 5000.0f;

	glm::mat4 projectionMatrix;

	void createProjectionMatrix() {
		float aspectRatio = 1920.0f / 1080.0f;
		float y_scale = (float)((1.0f / tan((FOV / 2.0f) * glm::pi<float>() / 180.0f)) * aspectRatio);
		float x_scale = y_scale / aspectRatio;
		float frustum_length = FAR_PLANE - NEAR_PLANE;

		projectionMatrix = glm::mat4();
		projectionMatrix[0][0] = x_scale;
		projectionMatrix[1][1] = y_scale;
		projectionMatrix[2][2] = -((FAR_PLANE + NEAR_PLANE) / frustum_length);
		projectionMatrix[2][3] = -1.0f;
		projectionMatrix[3][2] = -((2 * NEAR_PLANE * FAR_PLANE) / frustum_length);
		projectionMatrix[3][3] = 0.0f;
	}

public:
	MasterRenderer() {
		time = new Time();

		enableCulling();

		shader = new StaticShader();
		createProjectionMatrix();
		this->renderer = new EntityRenderer(shader, projectionMatrix);

		terrainShader = new TerrainShader();
		terrainRenderer = new TerrainRenderer(*terrainShader, projectionMatrix);
	}

	void enableCulling() {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	void disableCulling() {
		glDisable(GL_CULL_FACE);
	}

	void render(Light* sun, Camera* camera) {
		prepare();
		shader->start();
		shader->loadSkyColour(glm::vec3(101.0f / 255.0f, 150.0f / 255.0f, 206.0f / 255.0f));
		shader->loadLight(*sun);

		camera->move(Time::getInstance().getTimePassedFromLastCallMS() / 1000.0f);
		shader->loadViewMatrix(*camera);

		renderer->render(entities);

		shader->stop();

		terrainShader->start();
		terrainShader->loadSkyColour(glm::vec3(101.0f / 255.0f, 150.0f / 255.0f, 206.0f / 255.0f));
		terrainShader->loadLight(*sun);
		terrainShader->loadViewMatrix(*camera);

		terrainRenderer->render(terrains);

		terrainShader->stop();

		terrains.clear();
		entities.clear();
	}

	void processTerrain(Terrain& terrain) {
		terrains.push_back(&terrain);
	}

	void processEntity(Entity& entity) {
		TexturedModel* entityModel = entity.getModel();

		auto batch = entities.find(entityModel);
		if (batch == entities.end()) {
			std::vector<Entity*> newBatch;
			newBatch.push_back(&entity);
			entities.insert(std::make_pair(entityModel, newBatch));
		} else {
			batch->second.push_back(&entity);
		}
	}

	void cleanUp() {
		delete shader;
		delete renderer;
		delete terrainShader;
		delete terrainRenderer;
	}

	void prepare() {
		glEnable(GL_DEPTH_TEST);
		glClearColor(101.0f/255.0f, 150.0f / 255.0f, 206.0f / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	~MasterRenderer() {
		cleanUp();
	}
};