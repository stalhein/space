cmake_minimum_required(VERSION 3.16)
project(Space VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

file(GLOB_RECURSE SRC_FILES
    src/*.cpp
    external/glad/src/glad.c
)

add_executable(Space ${SRC_FILES})

target_include_directories(Space PRIVATE
    src
    include
    external/glad/include
)

find_package(OpenGL REQUIRED)
target_link_libraries(Space PRIVATE
    glfw
    ${OPENGL_LIBRARIES}
)

add_custom_target(run
    COMMAND $<TARGET_FILE:Space>
    DEPENDS Space
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

if(APPLE)
    target_link_libraries(Space PRIVATE "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
endif()

install(TARGETS Space DESTINATION bin)

set(CPACK_PACKAGE_NAME "Space")
set(CPACK_PACKAGE_VENDOR "Stalhein")
set(CPACK_GENERATOR "ZIP")

include(CPack)