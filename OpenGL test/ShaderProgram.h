#pragma once

#include "thirdparty/glew2/include/GL/glew.h"
#include <GL/GL.h>
#include "Maths.h"

class ShaderProgram {
	GLuint programID;
	GLuint vertexShaderID;
	GLuint fragmentShaderID;

	std::vector<std::string> attributes;
public:

	ShaderProgram(const char* vertexText, const char* fragmentText, std::vector<std::string> attributes) {
		vertexShaderID = loadShader(vertexText, GL_VERTEX_SHADER);
		fragmentShaderID = loadShader(fragmentText, GL_FRAGMENT_SHADER);
		this->attributes = attributes;

		programID = glCreateProgram();
		glAttachShader(programID, vertexShaderID);
		glAttachShader(programID, fragmentShaderID);
		bindAttributes();
		glLinkProgram(programID);
		glValidateProgram(programID); // too slow ?
	}

	virtual void getAllUniformLocation() = 0;

	GLuint getUniformLocation(std::string uniformName) {
		return glGetUniformLocation(programID, uniformName.c_str());
	}

	void start() {
		glUseProgram(programID);
	}

	void stop() {
		glUseProgram(0);
	}

	void cleanUp() {
		stop();
		glDetachShader(programID, vertexShaderID);
		glDetachShader(programID, fragmentShaderID);
		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);
		glDeleteProgram(programID);
	}

	void bindAttributes() {
		int index = 0;
		for (std::string attribute : attributes) {
			bindAttribute(index, attribute.c_str());
			index++;
		}
	}

	void bindAttribute(int& attribute, const char* variableName) {
		glBindAttribLocation(programID, attribute, variableName);
	}

	void loadFloat(GLuint& location, GLfloat& value) {
		glUniform1f(location, value);
	}

	void loadInt(GLuint& location, GLint value) {
		glUniform1i(location, value);
	}

	void loadVector(GLuint& location, glm::vec3& vector) {
		glUniform3f(location, vector.x, vector.y, vector.z);
	}

	void loadBool(GLuint& location, bool& value) {
		GLfloat fValue = 0.0f;
		if (value) fValue = 1.0f;
		glUniform1f(location, fValue);
	}

	void loadMatrix(GLuint& location, glm::mat4& matrix) {
		glUniformMatrix4fv(location, 1, false, glm::value_ptr(matrix));
	}

	GLuint loadShader(const char* shaderText, GLuint shaderType) {
		GLuint shaderID = glCreateShader(shaderType);
		glShaderSource(shaderID, 1, &shaderText, nullptr);
		glCompileShader(shaderID);
		GLint status = 0;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
		if (status  == GL_FALSE) {
			// Error;
		}
		return shaderID;
	}

	virtual ~ShaderProgram() = 0;
};

ShaderProgram::~ShaderProgram() {
	cleanUp();
}