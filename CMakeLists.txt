cmake_policy(VERSION 3.21)
cmake_minimum_required(VERSION 3.16...3.28)
project(Space VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Get dependencies
include(FetchContent)

message(STATUS "Fetching GLFW...")
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.8
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

message(STATUS "Fetching GLM...")
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG master
)
FetchContent_MakeAvailable(glm)

find_package(OpenGL REQUIRED)



file(GLOB_RECURSE SRC_FILES "src/*.cpp")

add_executable(Space ${SRC_FILES} external/glad/src/glad.c)

target_include_directories(Space PRIVATE
  src
  include
  external/glad/include
)

target_link_libraries(Space PRIVATE
  glfw
  OpenGL::GL
  glm::glm
)

if (WIN32)
  target_link_libraries(Space PRIVATE OpenGL32)
endif()

add_custom_target(run
  COMMAND Space
  DEPENDS Space
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
