#pragma once

#include "RawModel.h"
#include "WaterShader.h"
#include "WaterTile.h"
#include "WaterFrameBuffers.h"

class WaterRenderer {

	RawModel* quad;
	WaterShader* shader;
	WaterFrameBuffers* fbos;

	GLuint dudvTexture;
	GLuint normalMap;

	const float WAVE_SPEED = 0.03f;
	float moveFactor = 0.0f;

	void prepareRender(Camera& camera, Light* light) {
		shader->start();
		shader->loadViewMatrix(camera);

		moveFactor += WAVE_SPEED * Time::getInstance().getTimePassedFromLastCallS();
		if (moveFactor > 1.0f) moveFactor = moveFactor - 1.0f;
		shader->loadMoveFactor(moveFactor);

		shader->loadLight(light);

		GL_ERROR(glBindVertexArray(quad->getVaoID()));
		GL_ERROR(glEnableVertexAttribArray(0));
		GL_ERROR(glActiveTexture(GL_TEXTURE0));
		GL_ERROR(glBindTexture(GL_TEXTURE_2D, fbos->getReflectionTexture()));
		GL_ERROR(glActiveTexture(GL_TEXTURE1));
		GL_ERROR(glBindTexture(GL_TEXTURE_2D, fbos->getRefractionTexture()));
		GL_ERROR(glActiveTexture(GL_TEXTURE2));
		GL_ERROR(glBindTexture(GL_TEXTURE_2D, dudvTexture));
		GL_ERROR(glActiveTexture(GL_TEXTURE3));
		GL_ERROR(glBindTexture(GL_TEXTURE_2D, normalMap));
		GL_ERROR(glActiveTexture(GL_TEXTURE4));
		GL_ERROR(glBindTexture(GL_TEXTURE_2D, fbos->getRefractionDepthTexture()));

		GL_ERROR(glEnable(GL_BLEND));
		GL_ERROR(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	}

	void unbind() {
		GL_ERROR(glDisable(GL_BLEND));
		GL_ERROR(glDisableVertexAttribArray(0));
		GL_ERROR(glBindVertexArray(0));
		shader->stop();
	}

	void setUpVAO(Loader& loader) {
		// Just x and z vectex positions here, y is set to 0 in v.shader
		std::vector<float> vertices = { -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f };
		quad = loader.loadToVAO(vertices, 2);
	}

public:
	WaterRenderer(Loader& loader, WaterShader& shader, glm::mat4 projectionMatrix, WaterFrameBuffers& fbos, std::string dudvMapFile, std::string normalMapFile) {
		this->shader = &shader;
		this->fbos = &fbos;
		dudvTexture = loader.loadTexture(dudvMapFile);
		normalMap = loader.loadTexture(normalMapFile);

		this->shader->start();
		this->shader->connectTextureUnits();
		this->shader->loadProjectionMatrix(projectionMatrix);
		this->shader->stop();

		setUpVAO(loader);
	}

	void render(std::vector<WaterTile*>& water, Camera& camera, Light* light) {
		prepareRender(camera, light);
		for (auto tile : water) {
			glm::mat4 modelMatrix = createTransformationMatrix(
				glm::vec3(tile->getX(), tile->getHeight(), tile->getZ()), glm::vec3(0.0f, 0.0f, 0.0f),
				tile->getTileSize());
			shader->loadTransformationMatrix(modelMatrix);
			GL_ERROR(glDrawArrays(GL_TRIANGLES, 0, quad->getVertexCount()));
		}
		unbind();
	}
};