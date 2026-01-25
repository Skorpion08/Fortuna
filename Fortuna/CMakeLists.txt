# CMakeList.txt : CMake project for Fortuna, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (Fortuna "src/Core/App.h" "src/Core/App.cpp" "src/Core/Log.h" "src/Core/EntryPoint.cpp" "src/Platform/Window.h" "src/Platform/Window.cpp" "src/Platform/GUI.h" "src/Platform/GUI.cpp" "src/Renderer/Renderer.h" "src/Renderer/Renderer.cpp" "src/Renderer/OpenGL/RendererOpenGL.h" "src/Renderer/OpenGL/RendererOpenGL.cpp" "src/Renderer/Backend.h" "src/Core/SceneManager.h" "src/Core/SceneManager.cpp" "src/Core/Scenes/Scene.h" "src/Core/Scenes/MainMenuScene.h" "src/Core/Scenes/MainMenuScene.cpp" "src/Core/Scenes/GameScene.h" "src/Core/Scenes/GameScene.cpp" "src/Core/Scenes/EditorScene.h" "src/Core/Scenes/EditorScene.cpp" "src/Game/Question.h" "src/Game/Phase.h"  "src/Game/Wheel.h" "src/Game/Wheel.cpp" "src/Game/Slot.h" "src/Game/Slot.cpp"  "src/Game/Player.h" "src/Game/SpinResult.h")

target_include_directories(Fortuna PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(Fortuna PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/nlohmann)

# Set C++20
set_property(TARGET Fortuna PROPERTY CXX_STANDARD 20)
set_property(TARGET Fortuna PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Fortuna PROPERTY CXX_EXTENSIONS OFF)

# TODO: Add tests and install targets if needed.
# MSVC UTF-8 flag
if(MSVC)
    target_compile_options(Fortuna PRIVATE /utf-8)
endif()

target_link_libraries(Fortuna PRIVATE glfw glad fmt::fmt glm::glm imgui)

# Set compile definitions depending on build type
target_compile_definitions(Fortuna PRIVATE BACKEND_OPENGL)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Fortuna PRIVATE BACKEND_OPENGL LOG_CONSOLE_ENABLED LOG_FILE_ENABLED LOG_CRASH_ENABLED LOG_LEVEL=0)
elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    target_compile_definitions(Fortuna PRIVATE BACKEND_OPENGL LOG_FILE_ENABLED LOG_CRASH_ENABLED LOG_LEVEL=1)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(Fortuna PRIVATE BACKEND_OPENGL FILE_LOGGING LOG_CRASH_ENABLED LOG_LEVEL=2)
endif()