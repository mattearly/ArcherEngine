cmake_minimum_required(VERSION 3.29)

Project("ArcherEngine")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_TESTS "Set to OFF|ON (default is ON) to control build of ArcherEngine tests" ON)

if (ENABLE_TESTS)
  enable_testing()
endif()

option(ENABLE_SAMPLES "Set to OFF|ON (default is ON) to control build of ArcherEngine samples" ON)

add_subdirectory(Foundation)


