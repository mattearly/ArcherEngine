cmake_minimum_required(VERSION 3.22)
Project("ArcherEngine" VERSION 1.0)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


### LIBRARY COMPILE SETUP ###

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE PRIVATE_HEADERS "src/*.h")
file(GLOB_RECURSE PUBLIC_HEADERS "include/ArcherEngine/*.h")
add_library(${CMAKE_PROJECT_NAME} ${SOURCES} ${PRIVATE_HEADERS} ${PUBLIC_HEADERS})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include/ArcherEngine")
if (UNIX)
  find_package(GLEW REQUIRED)
  target_link_libraries(${CMAKE_PROJECT_NAME} glfw assimp imgui openal sndfile stb dl GL GLEW::GLEW)
  target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "/usr/include/imgui")
elseif (WIN32)
  find_package(imgui CONFIG REQUIRED)
  find_package(glm CONFIG REQUIRED)
  find_package(glew REQUIRED)
  find_package(glfw3 CONFIG REQUIRED)
  find_package(assimp CONFIG REQUIRED)
  find_package(OpenAL CONFIG REQUIRED)
  find_package(SndFile CONFIG REQUIRED)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glm::glm glfw assimp::assimp imgui::imgui OpenAL::OpenAL SndFile::sndfile GLEW::GLEW)
  find_path(STB_INCLUDE_DIRS "stb_c_lexer.h")
  target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${STB_INCLUDE_DIRS} ${imgui_INCLUDE_DIRS})
endif()


### LIBRARY INSTALL ###

install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION lib)
# public headers
install(DIRECTORY ${CMAKE_ROOT_DIR}/include/${CMAKE_PROJECT_NAME} DESTINATION include/${CMAKE_PROJECT_NAME})


### EXAMPLE USAGES ###

option(BUILD_EXAMPLES "build the example project(s) that use ${CMAKE_PROJECT_NAME} lib" ON)
if (${BUILD_EXAMPLES})
  add_subdirectory(examples)
endif()
