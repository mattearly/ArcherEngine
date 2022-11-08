cmake_minimum_required(VERSION 3.15)
Project("ArcherEngine" VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE ARCHER_SOURCES "src/*.cpp")
file(GLOB_RECURSE ARCHER_PRIVATE_HEADERS "src/*.h")
file(GLOB_RECURSE ARCHER_PUBLIC_HEADERS "include/ArcherEngine/*.h")

add_library(archer ${ARCHER_SOURCES} ${ARCHER_PRIVATE_HEADERS} ${ARCHER_PUBLIC_HEADERS})

target_include_directories(archer PRIVATE "${CMAKE_SOURCE_DIR}/include/ArcherEngine")

if (UNIX)
  find_package(GLEW REQUIRED)
  target_link_libraries(archer glfw assimp imgui openal sndfile stb dl GL GLEW::GLEW)
  target_include_directories(archer PRIVATE "/usr/include/imgui")
elseif (WIN32)
  find_package(imgui CONFIG REQUIRED)
  find_package(glm CONFIG REQUIRED)
  find_package(glew REQUIRED)
  find_package(glfw3 CONFIG REQUIRED)
  find_package(assimp CONFIG REQUIRED)
  find_package(OpenAL CONFIG REQUIRED)
  find_package(SndFile CONFIG REQUIRED)
  find_package(unofficial-nativefiledialog CONFIG REQUIRED)
  target_link_libraries(archer PRIVATE glm::glm glfw assimp::assimp imgui::imgui OpenAL::OpenAL SndFile::sndfile GLEW::GLEW unofficial::nativefiledialog::nfd)
  find_path(STB_INCLUDE_DIRS "stb_c_lexer.h")
  target_include_directories(archer PRIVATE ${STB_INCLUDE_DIRS} ${imgui_INCLUDE_DIRS})
endif()

option(BUILD_EXAMPLES "build the example project(s) that use archer lib" ON)

if (${BUILD_EXAMPLES})
  add_subdirectory(examples)
endif()
