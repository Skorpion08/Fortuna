#pragma once

#include <concepts>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <vector>

class Renderer
{
public:
    Renderer() {}

    virtual ~Renderer() {}

    virtual void SetClearColor(float r, float g, float b, float a) = 0;

    virtual void StartFrame() = 0;

    virtual void RenderWheel(int segmentCount, glm::vec2 center, float radius, float offsetAngle, const std::vector<glm::vec4>& colors) = 0;

    virtual void Resize(float width, float height) = 0;

    virtual void Render() = 0;
};
