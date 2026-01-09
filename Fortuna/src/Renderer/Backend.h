#pragma once

#if defined(BACKEND_OPENGL)
#include "OpenGL/RendererOpenGL.h"
#include "Renderer.h"
using ChosenBackend = RendererOpenGL;
#else
#error "No render backend selected"
#endif