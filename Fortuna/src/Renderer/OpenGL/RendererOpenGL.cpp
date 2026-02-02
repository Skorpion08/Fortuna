#include "RendererOpenGL.h"

#include "Core/Log.h"

#include "glad/glad.h"

#include <numbers>

#include <glm/gtc/matrix_transform.hpp>

#include "freetype/freetype.h"

#include "Game/Slot.h"

constexpr float PI = std::numbers::pi_v<float>;

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

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	projection = glm::ortho(-width / 2.0f, width / 2.0f, -height / 2.0f, height / 2.0f, -1.0f, 1.0f);
	program.SetMat4("projection", projection);

	Log::Info("Renderer's OpenGL backend initialized");

	Log::Info("Initializing text rendering...");

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		Log::Error("Failed to init FreeType");
		return;
	}

	FT_Face face;
	if (FT_New_Face(ft, "Fonts/Roboto/Roboto-VariableFont_wdth,wght.ttf", 0, &face))
	{
		Log::Error("Failed to load font");
		return;
	}

	FT_Set_Pixel_Sizes(face, 0, 26); // font height

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (unsigned char c = 32; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			Log::Error("Failed to load glyph {}", c);
			continue;
		}

		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Glyph glyph =
		{
			texture,
			{ face->glyph->bitmap.width, face->glyph->bitmap.rows },
			{ face->glyph->bitmap_left, face->glyph->bitmap_top },
			static_cast<unsigned int>(face->glyph->advance.x)
		};

		glyphs.insert({ c, glyph });
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	textProgram.Init("Shaders/textShader.vert", "Shaders/textShader.frag");
	textProgram.Use();

	glGenVertexArrays(1, &textVAO);
	glGenBuffers(1, &textVBO);

	glBindVertexArray(textVAO);

	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TextVertex) * 6, nullptr, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, pos));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, uv));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	textProgram.SetMat4("projection", projection);
}

RendererOpenGL::~RendererOpenGL()
{
	Log::Info("Shutting down OpenGL renderer");

	for (auto& [c, glyph] : glyphs)
	{
		glDeleteTextures(1, &glyph.textureID);
	}
	glyphs.clear();

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

void RendererOpenGL::RenderWheel(glm::vec2 center, float radius, float offsetAngle, const std::vector<std::unique_ptr<Slot>>& slots)
{
	float angleDiff = 2.0f * PI / static_cast<float>(slots.size());

	std::vector<Vertex> vertices;
	vertices.reserve(slots.size() * 3 + 3);

	for (int segment = 0; segment < slots.size(); ++segment)
	{
		float angle = segment * angleDiff;
		float startAngle = offsetAngle + angle;
		float endAngle = offsetAngle + angle + angleDiff;

		const glm::vec4& color = slots[segment]->GetColor();

		vertices.push_back({ center , color});
		vertices.push_back({ center + glm::vec2(cos(startAngle), sin(startAngle)) * radius , color});
		vertices.push_back({ center + glm::vec2(cos(endAngle), sin(endAngle)) * radius , color});
	}

	vertices.push_back({ center + glm::vec2(radius - 25.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)});
	vertices.push_back({ center + glm::vec2(radius, -10.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) });
	vertices.push_back({ center + glm::vec2(radius, 10.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) });

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertices.size(), vertices.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	program.Use();
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	for (int segment = 0; segment < slots.size(); ++segment)
	{
		float angle = offsetAngle + segment * angleDiff + (1.0f/3.0f) * angleDiff;

		RenderText(slots[segment]->GetName(), 200.0f, 0.0f, 1.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), angle, glm::vec2(0.0f));
	}
}

void RendererOpenGL::RenderText(const std::string& text, float x, float y, float scale, glm::vec4 color, float rotation, glm::vec2 rotationPoint)
{
	textProgram.Use();
	textProgram.SetMat4("projection", projection);
	textProgram.SetVec4("textColor", color);

	glm::mat4 model = glm::mat4(1.0f);

	// 1. Move origin to (x, y)
	model = glm::translate(model, glm::vec3(rotationPoint.x, rotationPoint.y, 0.0f));

	// 2. Apply rotation around Z
	model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, 1.0f));

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(textVAO);

	for (char c : text)
	{
		Glyph& g = glyphs[c];

		float xpos = x + g.bearing.x * scale;
		float ypos = y - (g.size.y - g.bearing.y) * scale;

		float w = g.size.x * scale;
		float h = g.size.y * scale;

		glm::vec2 topLeft = glm::vec2(model * glm::vec4(xpos, ypos + h, 0.0f, 1.0f));
		glm::vec2 bottomLeft = glm::vec2(model * glm::vec4(xpos, ypos, 0.0f, 1.0f));
		glm::vec2 bottomRight = glm::vec2(model * glm::vec4(xpos + w, ypos, 0.0f, 1.0f));
		glm::vec2 topRight = glm::vec2(model * glm::vec4(xpos + w, ypos + h, 0.0f, 1.0f));

		TextVertex vertices[6] =
		{
			{ topLeft,     {0.0f, 0.0f} },
			{ bottomLeft,  {0.0f, 1.0f} },
			{ bottomRight, {1.0f, 1.0f} },

			{ topLeft,     {0.0f, 0.0f} },
			{ bottomRight, {1.0f, 1.0f} },
			{ topRight,    {1.0f, 0.0f} }
		};

		glBindTexture(GL_TEXTURE_2D, g.textureID);

		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (g.advance >> 6) * scale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
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