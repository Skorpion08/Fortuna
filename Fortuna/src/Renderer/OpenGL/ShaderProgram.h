#pragma once

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

typedef unsigned int GLuint;

class ShaderProgram
{
public:
    void Init(const char* vertexShaderPath, const char* fragmentShaderPath);

    void Destroy();

    void Use() const;

    void SetBool(const char* name, bool value) const;

    void SetInt(const char* name, int value) const;

    void SetFloat(const char* name, float value) const;

    void SetVec2(const char* name, glm::vec2 value) const;

    void SetMat4(const char* name, const glm::mat4& value) const;

private:

    GLuint CreateShader(const char* path, GLuint shaderType);

private:

    GLuint shaderProgram;
};