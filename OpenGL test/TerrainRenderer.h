#pragma once

#include "TerrainShader.h"
#include "Terrain.h"
#include "TexturedModel.h"

class TerrainRenderer {
	TerrainShader* shader;

	void prepareTerrain(Terrain& terrain) {
		RawModel* rawmodel = terrain.getModel();

		glBindVertexArray(rawmodel->getVaoID());
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		bindTextures(terrain);
		shader->loadShineVariables(1, 0);
	}

	void bindTextures(Terrain& terrain) {
		TerrainTexturePack* texturePack = terrain.getTexturePack();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturePack->getBackgroundTexture()->getTextureID());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texturePack->getRTexture()->getTextureID());
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texturePack->getGTexture()->getTextureID());
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, texturePack->getBTexture()->getTextureID());
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, terrain.getBlendMap()->getTextureID());
	}

	void unbindTexturedModel() {
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glBindVertexArray(0);
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

			glDrawElements(GL_TRIANGLES, terrain->getModel()->getVertexCount(), GL_UNSIGNED_INT, 0);

			unbindTexturedModel();
		}
	}
};