cmake_policy(VERSION 3.21)
cmake_minimum_required(VERSION 3.16...3.28)
project(Space VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find installed packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

file(GLOB_RECURSE SRC_FILES
    "src/*.cpp"
    "external/glad/src/glad.c"
)

add_executable(Space ${SRC_FILES})

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

if(WIN32)
    target_link_libraries(Space PRIVATE opengl32)
endif()

add_custom_target(run
    COMMAND Space
    DEPENDS Space
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)