#pragma once

#include <glm/vec2.hpp>

#include "../Renderer.h"
#include "ShaderProgram.h"

struct Vertex
{
	glm::vec2 pos;
	glm::vec4 color;
};

constexpr unsigned int maxVertices = 256;

class RendererOpenGL : public Renderer
{
public:
	RendererOpenGL(float width, float height);

	virtual ~RendererOpenGL();

	virtual void SetClearColor(float r, float g, float b, float a) override;

	virtual void StartFrame() override;

	virtual void RenderWheel(int segmentCount, glm::vec2 center, float radius, float offsetAngle, const std::vector<glm::vec4>& colors) override;

	virtual void Resize(float width, float height) override;

	virtual void Render() override;

private:
	unsigned int vao;
	unsigned int vbo;

	int currentSegmentCount = 0;

	glm::mat4 projection;

	ShaderProgram program;
};