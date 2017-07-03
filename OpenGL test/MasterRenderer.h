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

	glm::vec3 skyColour;
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
	MasterRenderer(glm::vec3 skyColour) {
		time = new Time();
		this->skyColour = skyColour;

		enableCulling();

		shader = new StaticShader();
		createProjectionMatrix();
		this->renderer = new EntityRenderer(shader, projectionMatrix);

		terrainShader = new TerrainShader();
		terrainRenderer = new TerrainRenderer(*terrainShader, projectionMatrix);
	}

	void enableCulling() {
		GL_ERROR(glEnable(GL_CULL_FACE));
		GL_ERROR(glCullFace(GL_BACK));
	}

	void disableCulling() {
		GL_ERROR(glDisable(GL_CULL_FACE));
	}

	void render(Light* sun, Camera* camera, glm::vec3 skyColour) {
		this->skyColour = skyColour;
		prepare();
		shader->start();
		shader->loadSkyColour(skyColour);
		shader->loadLight(*sun);

		camera->move(Time::getInstance().getTimePassedFromLastCallMS() / 1000.0f);
		shader->loadViewMatrix(*camera);

		renderer->render(entities);

		shader->stop();

		terrainShader->start();
		terrainShader->loadSkyColour(skyColour);
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
		GL_ERROR(glEnable(GL_DEPTH_TEST));
		
		GL_ERROR(glClearColor(skyColour.x, skyColour.y, skyColour.z, 1.0f));
		GLenum error_ = glGetError();
		error_;
		GL_ERROR(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}

	~MasterRenderer() {
		cleanUp();
	}
};