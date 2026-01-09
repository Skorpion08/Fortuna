#pragma once

#include <concepts>

template <typename T>
concept RendererBackend =
    requires(T t)
{
    { t.Init() } -> std::same_as<void>;
    { t.Shutdown() } -> std::same_as<void>;
    { t.SetClearColor(0.0f, 0.0f, 0.0f, 1.0f) } -> std::same_as<void>;
    { t.StartFrame() } -> std::same_as<void>;
    { t.Render() } -> std::same_as<void>;
};

template <RendererBackend Backend>
class Renderer
{
public:
    Renderer() { backend.Init(); }

    ~Renderer() { backend.Shutdown(); }

    void SetClearColor(float r, float g, float b, float a) { backend.SetClearColor(r, g, b, a); }

    void StartFrame() { backend.StartFrame(); }

    void Render() { backend.Render(); }
    
private:
    Backend backend;
};