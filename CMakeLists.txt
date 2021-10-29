cmake_minimum_required(VERSION 3.16)
project(minimal_gl)

include(FetchContent)

FetchContent_Declare(glfw3 GIT_REPOSITORY https://github.com/glfw/glfw.git)
FetchContent_MakeAvailable(glfw3)

FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git)
FetchContent_MakeAvailable(glm)

add_library(glad "ext/glad/src/glad.c")
target_include_directories(glad PUBLIC "ext/glad/include")

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE "ext/stb")

add_executable(minimal_gl WIN32 "src/main.cpp")
target_link_libraries(minimal_gl PUBLIC glfw glad glm stb)

set_target_properties(minimal_gl PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/data")
