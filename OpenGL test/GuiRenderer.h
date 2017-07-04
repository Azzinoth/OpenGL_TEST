#pragma once

#include "GuiTexture.h"
#include "GuiShader.h"
#include "Loader.h"
#include "Checker.h"

class GuiRenderer {

	const RawModel* quad;
	GuiShader* shader;

public:
	GuiRenderer(Loader& loader) {
		std::vector<float> positions = { -1, 1, -1, -1, 1, 1, 1, -1 };
		quad = loader.loadToVAO(positions);
		shader = new GuiShader();
	}

	void render(std::vector<GuiTexture*>& quis) {
		shader->start();
		GL_ERROR(glBindVertexArray(quad->getVaoID()));
		GL_ERROR(glEnableVertexAttribArray(0));

		GL_ERROR(glEnable(GL_BLEND));
		GL_ERROR(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		GL_ERROR(glDisable(GL_DEPTH_TEST));

		for (auto gui : quis) {
			GL_ERROR(glActiveTexture(GL_TEXTURE0));
			GL_ERROR(glBindTexture(GL_TEXTURE_2D, gui->getTexture()));

			shader->loadTransformationMatrix(createTransformationMatrix(gui->getPosition(), gui->getScale()));
			GL_ERROR(glDrawArrays(GL_TRIANGLE_STRIP, 0, quad->getVertexCount()));
		}

		GL_ERROR(glDisable(GL_BLEND));
		GL_ERROR(glEnable(GL_DEPTH_TEST));

		GL_ERROR(glDisableVertexAttribArray(0));
		GL_ERROR(glBindVertexArray(0));
		shader->stop();
	}

	void cleanUp() {
		delete quad;
		delete shader;
	}

	~GuiRenderer() {
		cleanUp();
	}
};