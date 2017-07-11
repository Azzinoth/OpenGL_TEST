#pragma once

#include "RawModel.h"
#include "WaterShader.h"
#include "WaterTile.h"

class WaterRenderer {

	RawModel* quad;
	WaterShader* shader;

	void prepareRender(Camera& camera) {
		shader->start();
		shader->loadViewMatrix(camera);
		GL_ERROR(glBindVertexArray(quad->getVaoID()));
		GL_ERROR(glEnableVertexAttribArray(0));
	}

	void unbind() {
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
	WaterRenderer(Loader& loader, WaterShader& shader, glm::mat4 projectionMatrix) {
		this->shader = &shader;
		shader.start();
		shader.loadProjectionMatrix(projectionMatrix);
		shader.stop();
		setUpVAO(loader);
	}

	void render(std::vector<WaterTile*>& water, Camera& camera) {
		prepareRender(camera);
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