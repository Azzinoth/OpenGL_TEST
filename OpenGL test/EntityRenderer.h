#pragma once

#include "Entity.h"
#include "StaticShader.h"
#include "Checker.h"
#include "BoundingboxShader.h"
#include <map>

class EntityRenderer {

	StaticShader* shader;
	BoundingboxShader* boundingboxShader;

	void prepareTexturedModel(TexturedModel& model) {
		RawModel* rawmodel = model.getRawModel();

		GL_ERROR(glBindVertexArray(rawmodel->getVaoID()));
		GL_ERROR(glEnableVertexAttribArray(0));
		GL_ERROR(glEnableVertexAttribArray(1));
		GL_ERROR(glEnableVertexAttribArray(2));

		ModelTexture* texture = model.getTexture();
		int x = texture->getNumberOfRows();
		shader->loadNumberOfRows(x);
		if (texture->getHasTransparency()) {
			GL_ERROR(glDisable(GL_CULL_FACE));
		}
		else {
			GL_ERROR(glEnable(GL_CULL_FACE));
			GL_ERROR(glCullFace(GL_BACK));
		}

		shader->loadFakeLightingVariable(texture->getUseFakeLighting());
		shader->loadShineVariables(texture->getShineDamper(), texture->getReflectivity());

		GL_ERROR(glActiveTexture(GL_TEXTURE0));
		GL_ERROR(glBindTexture(GL_TEXTURE_2D, model.getTexture()->getID()));
	}

	void unbindTexturedModel() {
		GL_ERROR(glDisableVertexAttribArray(0));
		GL_ERROR(glDisableVertexAttribArray(1));
		GL_ERROR(glDisableVertexAttribArray(2));
		GL_ERROR(glBindVertexArray(0));
	}

	void prepareInstance(Entity& entity) {
		shader->loadTransformationMatrix(entity.getTransformationMatrix());
		shader->loadOffset(entity.getTextureXOffset(), entity.getTextureYOffset());
	}

public:

	EntityRenderer(StaticShader* shader, glm::mat4 projectionMatrix) {
		this->shader = shader;
		shader->start();
		shader->loadProjectionMatrix(projectionMatrix);
		shader->stop();

		boundingboxShader = new BoundingboxShader();
		boundingboxShader->start();
		boundingboxShader->loadProjectionMatrix(projectionMatrix);
		boundingboxShader->stop();
	}

	void render(std::map<TexturedModel*, std::vector<Entity*>>& entities, Camera* camera) {
		shader->start();

		std::vector<TexturedModel*> keys;
		for (auto model : entities)
		{
			auto alreadyInVector = [&](TexturedModel* model) -> bool {
				for (auto key : keys) {
					if (model == key) {
						return true;
					}
				}

				return false;
			};

			if (!alreadyInVector(model.first)) keys.push_back(model.first);
		}

		for (auto key : keys) {
			prepareTexturedModel(*key);
			for (auto batch : entities[key]) {
				prepareInstance(*batch);
				//GL_ERROR(glDrawElements(GL_TRIANGLES, key->getRawModel()->getVertexCount(), GL_UNSIGNED_INT, 0));
			}
			unbindTexturedModel();
		}

		shader->stop();
		boundingboxShader->start();
		boundingboxShader->loadViewMatrix(*camera);
		boundingboxShader->loadColor(glm::vec3(0.0, 1.0, 0.0));

		// bounding boxes
		for (auto key : keys) {
			for (auto batch : entities[key]) {
				boundingboxShader->loadTransformationMatrix(batch->getTransformationMatrix());

				GL_ERROR(glBindVertexArray(batch->getBoundingbox()->getVaoID()));
				GL_ERROR(glEnableVertexAttribArray(0));

				GL_ERROR(glDrawArrays(GL_TRIANGLES, 0, batch->getBoundingbox()->getVertexCount()));

				GL_ERROR(glDisableVertexAttribArray(0));
				GL_ERROR(glBindVertexArray(0));
			}
		}
		boundingboxShader->stop();
	}
};