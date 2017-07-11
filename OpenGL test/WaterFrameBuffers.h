#pragma once

#include "Checker.h"

class WaterFrameBuffers {

	const int REFLECTION_WIDTH = 320;
	const int REFLECTION_HEIGHT = 180;

	const int REFRACTION_WIDTH = 1280;
	const int REFRACTION_HEIGHT = 720;

	GLuint reflectionFrameBuffer;
	GLuint reflectionTexture;
	GLuint reflectionDepthBuffer;

	GLuint refractionFrameBuffer;
	GLuint refractionTexture;
	GLuint refractionDepthTexture;

	void initialiseReflectionFrameBuffer() {
		reflectionFrameBuffer = createFrameBuffer();
		reflectionTexture = createTextureAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
		reflectionDepthBuffer = createDepthBufferAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
		unbindCurrentFrameBuffer();
	}

	void initialiseRefractionFrameBuffer() {
		refractionFrameBuffer = createFrameBuffer();
		refractionTexture = createTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
		refractionDepthTexture = createDepthTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
		unbindCurrentFrameBuffer();
	}

	void bindFrameBuffer(int frameBuffer, int width, int height) {
		GL_ERROR(glBindTexture(GL_TEXTURE_2D, 0));//To make sure the texture isn't bound
		GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer));
		GL_ERROR(glViewport(0, 0, width, height));
	}

	GLuint createFrameBuffer() {
		GLuint frameBuffer;
		GL_ERROR(glGenFramebuffers(1, &frameBuffer));
		//generate name for frame buffer
		GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer));
		//create the framebuffer
		GL_ERROR(glDrawBuffer(GL_COLOR_ATTACHMENT0));
		//indicate that we will always render to color attachment 0
		return frameBuffer;
	}

	GLuint createTextureAttachment(int width, int height) {
		GLuint texture;
		GL_ERROR(glGenTextures(1, &texture));
		GL_ERROR(glBindTexture(GL_TEXTURE_2D, texture));
		GL_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
			0, GL_RGB, GL_UNSIGNED_BYTE, nullptr));
		GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GL_ERROR(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			texture, 0));
		return texture;
	}

	GLuint createDepthTextureAttachment(int width, int height) {
		GLuint texture;
		GL_ERROR(glGenTextures(1, &texture));
		GL_ERROR(glBindTexture(GL_TEXTURE_2D, texture));
		GL_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height,
			0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));
		GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GL_ERROR(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			texture, 0));
		return texture;
	}

	GLuint createDepthBufferAttachment(int width, int height) {
		GLuint depthBuffer;
		GL_ERROR(glGenRenderbuffers(1, &depthBuffer));
		GL_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer));
		GL_ERROR(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width,
			height));
		GL_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_RENDERBUFFER, depthBuffer));
		return depthBuffer;
	}

public:

	WaterFrameBuffers() {//call when loading the game
		initialiseReflectionFrameBuffer();
		initialiseRefractionFrameBuffer();
	}

	void cleanUp() {//call when closing the game
		GL_ERROR(glDeleteFramebuffers(1, &reflectionFrameBuffer));
		GL_ERROR(glDeleteTextures(1, &reflectionTexture));
		GL_ERROR(glDeleteRenderbuffers(1, &reflectionDepthBuffer));
		GL_ERROR(glDeleteFramebuffers(1, &refractionFrameBuffer));
		GL_ERROR(glDeleteTextures(1, &refractionTexture));
		GL_ERROR(glDeleteTextures(1, &refractionDepthTexture));
	}

	void bindReflectionFrameBuffer() {//call before rendering to this FBO
		bindFrameBuffer(reflectionFrameBuffer, REFLECTION_WIDTH, REFLECTION_HEIGHT);
	}

	void bindRefractionFrameBuffer() {//call before rendering to this FBO
		bindFrameBuffer(refractionFrameBuffer, REFRACTION_WIDTH, REFRACTION_HEIGHT);
	}

	void unbindCurrentFrameBuffer() {//call to switch to default frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 1904.0f, 1041.0f);
	}
	 
	int getReflectionTexture() {//get the resulting texture
		return reflectionTexture;
	}

	int getRefractionTexture() {//get the resulting texture
		return refractionTexture;
	}

	int getRefractionDepthTexture() {//get the resulting depth texture
		return refractionDepthTexture;
	}

	~WaterFrameBuffers() {
		cleanUp();
	}
};