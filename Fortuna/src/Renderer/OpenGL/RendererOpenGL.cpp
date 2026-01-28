#include "RendererOpenGL.h"

#include "Core/Log.h"

#include "glad/glad.h"

#include <numbers>

#include <glm/gtc/matrix_transform.hpp>

constexpr float  PI_f = std::numbers::pi_v<float>;
constexpr double PI = std::numbers::pi;

RendererOpenGL::RendererOpenGL(float width, float height)
{
	Log::Info("Initializing renderer's OpenGL backend...");

	SetClearColor(0.4f, 0.5f, 0.6f, 1.0f);

	program.Init("Shaders/shader.vert", "Shaders/shader.frag");
	program.Use();

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * maxVertices, nullptr, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	projection = glm::ortho(-width / 2.0f, width / 2.0f, -height / 2.0f, height / 2.0f, -1.0f, 1.0f);
	program.SetMat4("projection", projection);

	Log::Info("Renderer's OpenGL backend initialized");
}

RendererOpenGL::~RendererOpenGL()
{
	Log::Info("Shutting down OpenGL renderer");

	program.Destroy();

	Log::Info("OpenGL renderer shutdown");
}

void RendererOpenGL::SetClearColor(float r, float g, float b, float a)
{
	Log::Info("Clear color set to: {}, {}, {}, {}", r, g, b, a);

	glClearColor(r, g, b, a);
}

void RendererOpenGL::StartFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RendererOpenGL::RenderWheel(int segmentCount, glm::vec2 center, float radius, float offsetAngle, const std::vector<glm::vec4>& colors)
{
	assert(segmentCount == colors.size());

	float angleDiff = 2.0f * PI_f / static_cast<float>(segmentCount);

	std::vector<Vertex> vertices;
	vertices.reserve(segmentCount * 3);

	for (int segment = 0; segment < segmentCount; ++segment)
	{
		float angle = segment * angleDiff;
		float startAngle = offsetAngle + angle;
		float endAngle = offsetAngle + angle + angleDiff;

		const glm::vec4& color = colors[segment];

		vertices.push_back({ center , color});
		vertices.push_back({ center + glm::vec2(cos(startAngle), sin(startAngle)) * radius , color});
		vertices.push_back({ center + glm::vec2(cos(endAngle), sin(endAngle)) * radius , color});
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertices.size(), vertices.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	program.Use();
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

void RendererOpenGL::Resize(float width, float height)
{
	program.Use();
	projection = glm::ortho(-width / 2.0f, width / 2.0f, -height / 2.0f, height / 2.0f, -1.0f, 1.0f);
	program.SetMat4("projection", projection);
}

void RendererOpenGL::Render()
{

}