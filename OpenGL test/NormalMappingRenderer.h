#pragma once

#include "NormalMappingShader.h"

class NormalMappingRenderer {

	NormalMappingShader* shader;

	void prepareTexturedModel(TexturedModel& model) {
		RawModel* rawmodel = model.getRawModel();

		GL_ERROR(glBindVertexArray(rawmodel->getVaoID()));
		GL_ERROR(glEnableVertexAttribArray(0));
		GL_ERROR(glEnableVertexAttribArray(1));
		GL_ERROR(glEnableVertexAttribArray(2));
		GL_ERROR(glEnableVertexAttribArray(3));

		ModelTexture* texture = model.getTexture();
		shader->loadNumberOfRows(texture->getNumberOfRows());
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
		GL_ERROR(glActiveTexture(GL_TEXTURE1));
		GL_ERROR(glBindTexture(GL_TEXTURE_2D, model.getTexture()->getNormalMaptextureID()));
	}

	void unbindTexturedModel() {
		GL_ERROR(glDisableVertexAttribArray(0));
		GL_ERROR(glDisableVertexAttribArray(1));
		GL_ERROR(glDisableVertexAttribArray(2));
		GL_ERROR(glDisableVertexAttribArray(3));
		GL_ERROR(glBindVertexArray(0));
	}

	void prepareInstance(Entity& entity) {
		shader->loadTransformationMatrix(entity.getTransformationMatrix());
		shader->loadOffset(entity.getTextureXOffset(), entity.getTextureYOffset());
	}

	void prepare(glm::vec4 clipPlane, std::vector<Light*>& lights, Camera& camera) {
		shader->loadclipPlane(clipPlane);
		shader->loadSkyColour(glm::vec3(200.0f / 255.0f, 200.0f / 255.0f, 220.0f / 255.0f));

		shader->loadLights(lights, createViewMatrix(camera));
		shader->loadViewMatrix(createViewMatrix(camera));
	}

public:

	NormalMappingRenderer(glm::mat4 projectionMatrix) {
		this->shader = new NormalMappingShader();
		shader->start();
		shader->loadProjectionMatrix(projectionMatrix);
		shader->connectTextureUnits();
		shader->stop();
	}

	void render(std::map<TexturedModel*, std::vector<Entity*>>& entities, glm::vec4 clipPlane, std::vector<Light*>& lights, Camera& camera) {
		shader->start();
		prepare(clipPlane, lights, camera);
		/*for (TexturedModel model : entities.keySet()) {
			prepareTexturedModel(model);
			List<Entity> batch = entities.get(model);
			for (Entity entity : batch) {
				prepareInstance(entity);
				glDrawElements(GL_TRIANGLES, model.getRawModel().getVertexCount(), GL_UNSIGNED_INT, 0);
			}
			unbindTexturedModel();
		}*/

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
				GL_ERROR(glDrawElements(GL_TRIANGLES, key->getRawModel()->getVertexCount(), GL_UNSIGNED_INT, 0));
			}
			unbindTexturedModel();
		}

		shader->stop();
	}

	void cleanUp() {
		shader->cleanUp();
	}
};