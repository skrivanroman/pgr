cmake_minimum_required(VERSION 3.20)
project(pgr LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(WARNING "This project is configured for Clang. You are using: ${CMAKE_CXX_COMPILER_ID}")
endif()

if (WIN32)
    add_compile_definitions(PLATFORM_WINDOWS)
elseif(UNIX AND NOT APPLE)
    add_compile_definitions(PLATFORM_LINUX)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/debug/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release/bin)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.cpp
    src/*.c
    src/*.hpp
    src/*.h
)

add_subdirectory(vendor/glfw)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/assimp)

add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    glad
    assimp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    vendor/glm
    vendor/stb
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    ASSET_PATH="${CMAKE_SOURCE_DIR}/assets"
)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wconversion
    -Wsign-conversion
    -Wold-style-cast
    -Wdouble-promotion
    -Wformat=2
    -Wimplicit-fallthrough
    -Wnull-dereference
)

set(CMAKE_CXX_FLAGS_DEBUG "-O1 -g3 -fno-omit-frame-pointer -fno-optimize-sibling-calls")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_BUILD)

    target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address,undefined)
    target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address,undefined)

    target_compile_options(${PROJECT_NAME} PRIVATE -glldb)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -flto")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME} PRIVATE RELEASE_BUILD)
    target_link_options(${PROJECT_NAME} PRIVATE -flto)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -ffast-math
    )
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
