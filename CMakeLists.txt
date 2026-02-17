cmake_policy(VERSION 3.21)
cmake_minimum_required(VERSION 3.16...3.28)
project(Space VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)
set(GLFW_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.9.8
)
FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
    FetchContent_Populate(glm)
endif()

# OpenGL
find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SRC_FILES
    "src/*.cpp"
    "external/glad/src/glad.c"
)

add_executable(Space ${SRC_FILES})

target_include_directories(Space PRIVATE
    src
    include
    external/glad/include
    ${glm_SOURCE_DIR}
)

target_link_libraries(Space PRIVATE 
    glfw
    OpenGL::GL
)

if(WIN32)
    target_link_libraries(Space PRIVATE opengl32)
endif()

add_custom_target(run
    COMMAND Space
    DEPENDS Space
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)