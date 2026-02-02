#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <vector>

#include <string>
#include <memory>

class Slot;

class Renderer
{
public:
    Renderer() {}

    virtual ~Renderer() {}

    virtual void SetClearColor(float r, float g, float b, float a) = 0;

    virtual void StartFrame() = 0;

    virtual void RenderWheel(glm::vec2 center, float radius, float offsetAngle, const std::vector<std::unique_ptr<Slot>>& slots) = 0;

    virtual void RenderText(const std::string& text, float x, float y, float scale, glm::vec4 color, float rotation, glm::vec2 rotationPoint) = 0;

    virtual void Resize(float width, float height) = 0;

    virtual void Render() = 0;
};
