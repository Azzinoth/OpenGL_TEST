#pragma once

#include "TerrainShader.h"
#include "Terrain.h"
#include "TexturedModel.h"

class TerrainRenderer {
	TerrainShader* shader;

	void prepareTerrain(Terrain& terrain) {
		RawModel* rawmodel = terrain.getModel();

		GL_ERROR(glBindVertexArray(rawmodel->getVaoID()));
		GL_ERROR(glEnableVertexAttribArray(0));
		GL_ERROR(glEnableVertexAttribArray(1));
		GL_ERROR(glEnableVertexAttribArray(2));

		bindTextures(terrain);
		shader->loadShineVariables(1, 0);
	}

	void bindTextures(Terrain& terrain) {
		TerrainTexturePack* texturePack = terrain.getTexturePack();

		GL_ERROR(glActiveTexture(GL_TEXTURE0));
		GL_ERROR(glBindTexture(GL_TEXTURE_2D, texturePack->getBackgroundTexture()->getTextureID()));
		GL_ERROR(glActiveTexture(GL_TEXTURE1));
		GL_ERROR(glBindTexture(GL_TEXTURE_2D, texturePack->getRTexture()->getTextureID()));
		GL_ERROR(glActiveTexture(GL_TEXTURE2));
		GL_ERROR(glBindTexture(GL_TEXTURE_2D, texturePack->getGTexture()->getTextureID()));
		GL_ERROR(glActiveTexture(GL_TEXTURE3));
		GL_ERROR(glBindTexture(GL_TEXTURE_2D, texturePack->getBTexture()->getTextureID()));
		GL_ERROR(glActiveTexture(GL_TEXTURE4));
		GL_ERROR(glBindTexture(GL_TEXTURE_2D, terrain.getBlendMap()->getTextureID()));
	}

	void unbindTexturedModel() {
		GL_ERROR(glDisableVertexAttribArray(0));
		GL_ERROR(glDisableVertexAttribArray(1));
		GL_ERROR(glDisableVertexAttribArray(2));
		GL_ERROR(glBindVertexArray(0));
	}

	void loadModelMatrix(Terrain& terrain) {
		shader->loadTransformationMatrix(
			createTransformationMatrix(glm::vec3(terrain.getX(), 0.0f, terrain.getZ()),
									   glm::vec3(0.0f), 2.0f));
	}

public:
	TerrainRenderer(TerrainShader& shader, glm::mat4 projectionMatrix) {
		this->shader = &shader;
		this->shader->start();
		this->shader->loadProjectionMatrix(projectionMatrix);
		this->shader->connectTextureUnits();
		this->shader->stop();
	}

	void render(std::vector<Terrain*>& terrains) {
		for (auto terrain : terrains) {
			prepareTerrain(*terrain);
			loadModelMatrix(*terrain);

			GL_ERROR(glDrawElements(GL_TRIANGLES, terrain->getModel()->getVertexCount(), GL_UNSIGNED_INT, 0));

			unbindTexturedModel();
		}
	}
};