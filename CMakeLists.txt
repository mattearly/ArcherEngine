cmake_minimum_required(VERSION 3.12)
Project("ArcherEngine" VERSION 1.0)

file(GLOB_RECURSE SOURCES  "src/*.cpp")
file(GLOB_RECURSE PRIVATE_HEADERS "src/*.h")
file(GLOB_RECURSE PUBLIC_HEADERS "include/ArcherEngine/*.h")

add_library(archer ${SOURCES} ${PRIVATE_HEADERS} ${PUBLIC_HEADERS})

target_include_directories(archer PRIVATE "${CMAKE_SOURCE_DIR}/include/ArcherEngine")

if (UNIX)
  find_package(GLEW REQUIRED)
  target_link_libraries(archer glfw assimp imgui openal sndfile stb dl GL GLEW::GLEW)
  target_include_directories(archer PRIVATE "/usr/include/imgui")
endif()

option(BUILD_EXAMPLES "build the example project(s) that use archer lib" ON)

if (${BUILD_EXAMPLES})
  add_subdirectory(examples)
endif()
