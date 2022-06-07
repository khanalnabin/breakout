#include "../include/shader.h"

#include <iostream>
Shader::Shader() {}

Shader &Shader::Use() {
	glUseProgram(this->ID);
	return *this;
}

void Shader::Compile(const char *vertexSource, const char *fragmentSource,
                     const char *geometrySource) {
	unsigned int sVertex, sFragment, sGeometry;

	// vertex shader
	sVertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(sVertex, 1, &vertexSource, NULL);
	glCompileShader(sVertex);
	checkCompileErrors(sVertex, "VERTEX");

	// fragment shader
	sFragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(sFragment, 1, &fragmentSource, NULL);
	glCompileShader(sFragment);
	checkCompileErrors(sFragment, "FRAGMENT");

	// if geometry shader is also provide, compile it
	if (geometrySource != NULL) {
		sGeometry = glCreateShader(GL_GEOMETRY_SHADER);
		glCompileShader(sGeometry);
		checkCompileErrors(sGeometry, "GEOMETRY");
	}

	this->ID = glCreateProgram();
	glAttachShader(this->ID, sVertex);
	glAttachShader(this->ID, sFragment);
	if (geometrySource != NULL)
		glAttachShader(this->ID, sGeometry);
	glLinkProgram(this->ID);
	checkCompileErrors(this->ID, "PROGRAM");

	glDeleteShader(sVertex);
	glDeleteShader(sFragment);
	if (geometrySource != NULL)
		glDeleteShader(sGeometry);
}
void Shader::SetFloat(const char *name, float value, bool useShader) {
	if (useShader)
		this->Use();
	glUniform1f(glGetUniformLocation(this->ID, name), value);
}
void Shader::SetInteger(const char *name, int value, bool useShader) {
	if (useShader)
		this->Use();
	glUniform1i(glGetUniformLocation(this->ID, name), value);
}
void Shader::SetVector2f(const char *name, float x, float y, bool useShader) {
	if (useShader)
		this->Use();
	glUniform2f(glGetUniformLocation(this->ID, name), x, y);
}
void Shader::SetVector2f(const char *name, const glm::vec2 &value,
                         bool useShader) {
	if (useShader)
		this->Use();
	glUniform2f(glGetUniformLocation(this->ID, name), value.x, value.y);
}
void Shader::SetVector3f(const char *name, float x, float y, float z,
                         bool useShader) {
	if (useShader)
		this->Use();
	glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
}
void Shader::SetVector3f(const char *name, const glm::vec3 &value,
                         bool useShader) {
	if (useShader)
		this->Use();
	glUniform3f(glGetUniformLocation(this->ID, name), value.x, value.y,
	            value.z);
}
void Shader::SetVector4f(const char *name, float x, float y, float z, float w,
                         bool useShader) {
	if (useShader)
		this->Use();
	glUniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
}
void Shader::SetVector4f(const char *name, const glm::vec4 &value,
                         bool useShader) {
	if (useShader)
		this->Use();
	glUniform4f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z,
	            value.w);
}
void Shader::SetMatrix4(const char *name, const glm::mat4 &matrix,
                        bool useShader) {
	if (useShader)
		this->Use();
	glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, false,
	                   glm::value_ptr(matrix));
}

void Shader::checkCompileErrors(unsigned int object, std::string type) {
	int success;
	char infoLog[1024];
	if (type != "PROGRAM") {
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(object, 1024, NULL, infoLog);
			std::cout
			    << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
			    << infoLog
			    << "\n -- --------------------------------------------------- "
			       "-- "
			    << std::endl;
		}
	} else {
		glGetProgramiv(object, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(object, 1024, NULL, infoLog);
			std::cout
			    << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
			    << infoLog
			    << "\n -- --------------------------------------------------- "
			       "-- "
			    << std::endl;
		}
	}
}
