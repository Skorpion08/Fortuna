# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Fortuna")

# Include sub-projects.

add_subdirectory ("Dependencies/glfw")

add_subdirectory ("Dependencies/glad")

add_subdirectory ("Dependencies/glm")

add_subdirectory ("Dependencies/fmt")

add_subdirectory ("Dependencies/imgui")

set(FREETYPE_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/Dependencies/freetype/include")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(FREETYPE_LIBRARY "${CMAKE_SOURCE_DIR}/Dependencies/freetype/objs/x64/Debug/freetype.lib")
elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(FREETYPE_LIBRARY "${CMAKE_SOURCE_DIR}/Dependencies/freetype/objs/x64/Debug/freetype.lib")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(FREETYPE_LIBRARY "${CMAKE_SOURCE_DIR}/Dependencies/freetype/objs/x64/Release/freetype.lib")
endif()

#find_package(Freetype REQUIRED)

add_subdirectory ("Fortuna")

add_custom_target(copy_fonts ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/Fortuna/Fonts"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Fortuna/Fonts
        ${CMAKE_BINARY_DIR}/Fortuna/Fonts
)

add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/Fortuna/Shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Fortuna/Shaders
        ${CMAKE_BINARY_DIR}/Fortuna/Shaders
)