#pragma once

#include "RawModel.h"
#include "SkyboxShader.h"

class SkyboxRenderer {

	const float SIZE = 8000.0f;

	std::vector<float> VERTICES = {
		-SIZE,  SIZE, -SIZE,
		-SIZE, -SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,
		SIZE,  SIZE, -SIZE,
		-SIZE,  SIZE, -SIZE,

		-SIZE, -SIZE,  SIZE,
		-SIZE, -SIZE, -SIZE,
		-SIZE,  SIZE, -SIZE,
		-SIZE,  SIZE, -SIZE,
		-SIZE,  SIZE,  SIZE,
		-SIZE, -SIZE,  SIZE,

		SIZE, -SIZE, -SIZE,
		SIZE, -SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		SIZE,  SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,

		-SIZE, -SIZE,  SIZE,
		-SIZE,  SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		SIZE, -SIZE,  SIZE,
		-SIZE, -SIZE,  SIZE,

		-SIZE,  SIZE, -SIZE,
		SIZE,  SIZE, -SIZE,
		SIZE,  SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		-SIZE,  SIZE,  SIZE,
		-SIZE,  SIZE, -SIZE,

		-SIZE, -SIZE, -SIZE,
		-SIZE, -SIZE,  SIZE,
		SIZE, -SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,
		-SIZE, -SIZE,  SIZE,
		SIZE, -SIZE,  SIZE
	};

	RawModel* cube;
	GLuint texture;
	GLuint nightTexture;
	SkyboxShader* shader;
	int time = 0;

public:

	SkyboxRenderer(Loader& loader, glm::mat4& projectionMatrix, std::vector<std::string>& textureFiles, std::vector<std::string>& secondTextureFiles) {
		cube = loader.loadToVAO(VERTICES, 3);
		texture = loader.loadCubeMap(textureFiles);
		nightTexture = loader.loadCubeMap(secondTextureFiles);
		shader = new SkyboxShader();
		shader->start();
		shader->connectTextureUnits();
		shader->loadProjectionMatrix(projectionMatrix);
		shader->stop();
	}

	void render(Camera& camera, glm::vec3 fogColor) {
		shader->start();
		shader->loadViewMatrix(camera);
		shader->loadFogColor(fogColor);

		if (time >= 0 && time < 5000) {
			shader->loadFogColor(glm::vec3(0.1f, 0.1f, 0.1f));
		}
		else if (time >= 5000.0f && time < 8000.0f) {
			shader->loadFogColor(glm::vec3(0.1f, 0.1f, 0.1f));
		}
		else if (time >= 8000 && time < 21000) {
			
		}
		else {
			shader->loadFogColor(glm::vec3(0.1f, 0.1f, 0.1f));
		}


		GL_ERROR(glBindVertexArray(cube->getVaoID()));
		GL_ERROR(glEnableVertexAttribArray(0));
		bindTextures();
		GL_ERROR(glDrawArrays(GL_TRIANGLES, 0, cube->getVertexCount()));
		GL_ERROR(glDisableVertexAttribArray(0));
		GL_ERROR(glBindVertexArray(0));
		shader->stop();
	}

	void bindTextures() {
		time += Time::getInstance().getTimePassedFromLastCallMS()/*deltaTime*/;
		time %= 24000;

		GLuint texture1;
		GLuint texture2;
		float blendFactor;

		if (time >= 0 && time < 5000) {
			texture1 = nightTexture;
			texture2 = nightTexture;
			blendFactor = (time - 0.0f) / (5000.0f - 0.0f);
		}
		else if (time >= 5000.0f && time < 8000.0f) {
			texture1 = nightTexture;
			texture2 = texture;
			blendFactor = (time - 5000.0f) / (8000.0f - 5000.0f);
		}
		else if (time >= 8000 && time < 21000) {
			texture1 = texture;
			texture2 = texture;
			blendFactor = (time - 8000.0f) / (21000.0f - 8000.0f);
		}
		else {
			texture1 = texture;
			texture2 = nightTexture;
			blendFactor = (time - 21000.0f) / (24000.0f - 21000.0f);
		}

		GL_ERROR(glActiveTexture(GL_TEXTURE0));
		GL_ERROR(glBindTexture(GL_TEXTURE_CUBE_MAP, texture1));
		GL_ERROR(glActiveTexture(GL_TEXTURE1));
		GL_ERROR(glBindTexture(GL_TEXTURE_CUBE_MAP, texture2));
		shader->loadBlendFactor(blendFactor);
	}
};