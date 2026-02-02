#pragma once

#include <glm/vec2.hpp>

#include "../Renderer.h"
#include "ShaderProgram.h"

#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H

struct Vertex
{
	glm::vec2 pos;
	glm::vec4 color;
};

struct TextVertex
{
	glm::vec2 pos;
	glm::vec2 uv;
};

struct Glyph
{
	unsigned int textureID;   // glyph texture
	glm::ivec2 size;    // width, height
	glm::ivec2 bearing; // offset from baseline
	unsigned int advance;
};

constexpr unsigned int maxVertices = 256;

class RendererOpenGL : public Renderer
{
public:
	RendererOpenGL(float width, float height);

	virtual ~RendererOpenGL();

	virtual void SetClearColor(float r, float g, float b, float a) override;

	virtual void StartFrame() override;

	virtual void RenderWheel(glm::vec2 center, float radius, float offsetAngle, const std::vector<std::unique_ptr<Slot>>& slots) override;

	virtual void RenderText(const std::string& text, float x, float y, float scale, glm::vec4 color, float rotation, glm::vec2 rotationPoint) override;

	virtual void Resize(float width, float height) override;

	virtual void Render() override;

private:
	unsigned int vao;
	unsigned int vbo;

	unsigned int textVAO;
	unsigned int textVBO;

	int currentSegmentCount = 0;

	glm::mat4 projection;

	ShaderProgram program;
	ShaderProgram textProgram;

	std::unordered_map<char, Glyph> glyphs;
};