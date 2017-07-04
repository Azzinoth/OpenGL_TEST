#pragma once

#include "thirdparty/glew2/include/GL/glew.h"
#include <GL/GL.h>
#include "Maths.h"
#include "Checker.h"

class ShaderProgram {
	GLuint programID;
	GLuint vertexShaderID;
	GLuint fragmentShaderID;

	std::vector<std::string> attributes;
public:

	ShaderProgram(const char* vertexText, const char* fragmentText, std::vector<std::string>&& attributes) {
		vertexShaderID = loadShader(vertexText, GL_VERTEX_SHADER);
		fragmentShaderID = loadShader(fragmentText, GL_FRAGMENT_SHADER);
		this->attributes = attributes;

		GL_ERROR(programID = glCreateProgram());
		GL_ERROR(glAttachShader(programID, vertexShaderID));
		GL_ERROR(glAttachShader(programID, fragmentShaderID));
		bindAttributes();
		GL_ERROR(glLinkProgram(programID));
		GL_ERROR(glValidateProgram(programID)); // too slow ?

		GL_ERROR(glDeleteShader(vertexShaderID));
		GL_ERROR(glDeleteShader(fragmentShaderID));
	}

	virtual void getAllUniformLocation() = 0;

	GLuint getUniformLocation(std::string uniformName) {
		GLuint result = glGetUniformLocation(programID, uniformName.c_str());
		GL_ERROR(result);
		return result;
	}

	void start() {
		GL_ERROR(glUseProgram(programID));
	}

	void stop() {
		GL_ERROR(glUseProgram(0));
	}

	void cleanUp() {
		stop();
		//GL_ERROR(glDetachShader(programID, vertexShaderID));
		//GL_ERROR(glDetachShader(programID, fragmentShaderID));
		//GL_ERROR(glDeleteShader(vertexShaderID));
		//GL_ERROR(glDeleteShader(fragmentShaderID));
		GL_ERROR(glDeleteProgram(programID));
	}

	void bindAttributes() {
		int index = 0;
		for (std::string attribute : attributes) {
			bindAttribute(index, attribute.c_str());
			index++;
		}
	}

	void bindAttribute(int& attribute, const char* variableName) {
		GL_ERROR(glBindAttribLocation(programID, attribute, variableName));
	}

	void loadFloat(GLuint& location, GLfloat& value) {
		GL_ERROR(glUniform1f(location, value));
	}

	void loadInt(GLuint& location, GLint value) {
		GL_ERROR(glUniform1i(location, value));
	}

	void loadVector(GLuint& location, glm::vec3& vector) {
		GL_ERROR(glUniform3f(location, vector.x, vector.y, vector.z));
	}

	void load2DVector(GLuint& location, glm::vec2& vector) {
		GL_ERROR(glUniform2f(location, vector.x, vector.y));
	}

	void loadBool(GLuint& location, bool& value) {
		GLfloat fValue = 0.0f;
		if (value) fValue = 1.0f;
		GL_ERROR(glUniform1f(location, fValue));
	}

	void loadMatrix(GLuint& location, glm::mat4& matrix) {
		GL_ERROR(glUniformMatrix4fv(location, 1, false, glm::value_ptr(matrix)));
	}

	GLuint loadShader(const char* shaderText, GLuint shaderType) {
		GLuint shaderID;
		GL_ERROR(shaderID = glCreateShader(shaderType));
		GL_ERROR(glShaderSource(shaderID, 1, &shaderText, nullptr));
		GL_ERROR(glCompileShader(shaderID));
		GLint status = 0;
		GL_ERROR(glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status));

		return shaderID;
	}

	virtual ~ShaderProgram() = 0;
};

ShaderProgram::~ShaderProgram() { }