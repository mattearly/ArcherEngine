  

file(GLOB_RECURSE ARCHER_SOURCES "src/*.cpp")
file(GLOB_RECURSE ARCHER_PUBLIC_HEADERS "include/ArcherEngine/*.h")

add_library(archer ${ARCHER_SOURCES} ${ARCHER_PUBLIC_HEADERS})

target_include_directories(archer PUBLIC "include/ArcherEngine")

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
  find_package(nfd CONFIG REQUIRED)
  target_link_libraries(archer PRIVATE glm::glm glfw assimp::assimp imgui::imgui OpenAL::OpenAL SndFile::sndfile GLEW::GLEW nfd::nfd)
  find_path(STB_INCLUDE_DIRS "stb_c_lexer.h")
  target_include_directories(archer PRIVATE ${STB_INCLUDE_DIRS} ${imgui_INCLUDE_DIRS})
endif()

if (ENABLE_TESTS)
  add_subdirectory(testsuite)
  add_subdirectory(samples)
endif()

