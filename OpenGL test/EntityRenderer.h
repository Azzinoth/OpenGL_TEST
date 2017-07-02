#pragma once

#include "Entity.h"
#include "StaticShader.h"
#include <map>

class EntityRenderer {

	StaticShader* shader;

	void prepareTexturedModel(TexturedModel& model) {
		RawModel* rawmodel = model.getRawModel();

		glBindVertexArray(rawmodel->getVaoID());
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		ModelTexture* texture = model.getTexture();
		if (texture->getHasTransparency()) {
			glDisable(GL_CULL_FACE);
		}
		else {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}

		shader->loadFakeLightingVariable(texture->getUseFakeLighting());
		shader->loadShineVariables(texture->getShineDamper(), texture->getReflectivity());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, model.getTexture()->getID());
	}

	void unbindTexturedModel() {
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glBindVertexArray(0);
	}

	void prepareInstance(Entity& entity) {
		shader->loadTransformationMatrix(entity.getTransformationMatrix());
			//createTransformationMatrix(entity.getPosition(), entity.getRotation(), entity.getScale())); // it was a stupid code :)
	}

public:

	EntityRenderer(StaticShader* shader, glm::mat4 projectionMatrix) {
		this->shader = shader;
		shader->start();
		shader->loadProjectionMatrix(projectionMatrix);
		shader->stop();
	}

	void render(std::map<TexturedModel*, std::vector<Entity*>>& entities) {
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
				glDrawElements(GL_TRIANGLES, key->getRawModel()->getVertexCount(), GL_UNSIGNED_INT, 0);
			}
			unbindTexturedModel();
		}
	}
};