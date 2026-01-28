#include "ShaderProgram.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <stdexcept>
#include <string>
#include <sstream>
#include <fstream>

#include "Core/Log.h"

void ShaderProgram::Init(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	GLuint vertex = CreateShader(vertexShaderPath, GL_VERTEX_SHADER), 
		fragment = CreateShader(fragmentShaderPath, GL_FRAGMENT_SHADER);

	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertex);
	glAttachShader(shaderProgram, fragment);

	glLinkProgram(shaderProgram);

	// Linking error checking
	int success;
	char infoLog[1024];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 1024, nullptr, infoLog);
		Log::Error("Shader linking error");
		throw std::runtime_error(std::string("SHADER_LINKING_ERROR\n") + std::string(infoLog));
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

GLuint ShaderProgram::CreateShader(const char* path, GLuint shaderType)
{
	assert(shaderType == GL_VERTEX_SHADER || shaderType == GL_FRAGMENT_SHADER);

	std::string shaderCodeString;

	std::ifstream shaderFile;
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try // Read the file into the string
	{
		std::stringstream shaderStream;

		shaderFile.open(path);
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();

		shaderCodeString = shaderStream.str();

	}
	catch (std::ifstream::failure& e)
	{
		Log::Error("Shader file not successfully read, path: {}, type: {}, error: {}", std::string(path), (shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment"), e.what());
		throw std::runtime_error(fmt::format("Shader file not successfully read, path: {}, type: {}, error: {}", std::string(path), (shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment"), e.what()));
	}

	const char* shaderCode = shaderCodeString.c_str();

	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderCode, nullptr);
	glCompileShader(shader);

	// Compile error checking
	int success;
	char infoLog[1024];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
		Log::Error("{}_SHADER_COMPILE_ERROR: {}", (shaderType == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT"), infoLog);
		throw std::runtime_error((shaderType == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") + std::string("_SHADER_COMPILE_ERROR\n") + std::string(infoLog));
	}

	return shader;
}

void ShaderProgram::Destroy()
{
	glDeleteProgram(shaderProgram);
}

void ShaderProgram::Use() const
{
	glUseProgram(shaderProgram);
}

void ShaderProgram::SetBool(const char* name, bool value) const
{
    glUniform1i(glGetUniformLocation(shaderProgram, name), (int)value);
}

void ShaderProgram::SetInt(const char* name, int value) const
{
    glUniform1i(glGetUniformLocation(shaderProgram, name), value);
}

void ShaderProgram::SetFloat(const char* name, float value) const
{
    glUniform1f(glGetUniformLocation(shaderProgram, name), value);
}

void ShaderProgram::SetVec2(const char* name, glm::vec2 value) const
{
    glUniform2f(glGetUniformLocation(shaderProgram, name), value.x, value.y);
}

void ShaderProgram::SetMat4(const char* name, const glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name), 1, GL_FALSE, glm::value_ptr(value));
}
