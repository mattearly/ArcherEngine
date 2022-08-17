cmake_minimum_required(VERSION 3.12)
Project("ArcherEngine" VERSION 1.0)

file(GLOB_RECURSE SOURCES  "src/*.cpp")
file(GLOB_RECURSE PRIVATE_HEADERS "src/*.h")
file(GLOB_RECURSE PUBLIC_HEADERS "include/ArcherEngine/*.h")

add_library(archer ${SOURCES} ${PRIVATE_HEADERS} ${PUBLIC_HEADERS})

target_link_libraries(archer glm glad glfw assimp imgui openal sndfile stb curl gl)

target_include_directories(archer PRIVATE "${CMAKE_SOURCE_DIR}/include/ArcherEngine")

if (UNIX)
  target_include_directories(archer PRIVATE "/usr/include/imgui")
endif()
