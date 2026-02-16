cmake_minimum_required(VERSION 3.16)
project(Space VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
find_package(glfw3 3.3 REQUIRED)
target_link_libraries(Space PRIVATE
    OpenGL::GL
    glfw
)

add_custom_target(run
    COMMAND $<TARGET_FILE:Space>
    DEPENDS Space
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

install(TARGETS Space DESTINATION bin)

set(CPACK_PACKAGE_NAME "Space")
set(CPACK_PACKAGE_VENDOR "Stalhein")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Space simulator with the Sun, Earth and the Moon.")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

if (WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_SOURCE_GENERATOR "ZIP")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
elseif(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "External")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "erich.stalhein@proton.me")
    set(CPACK_GENERATOR "TGZ")
endif()

include(CPack)