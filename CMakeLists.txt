cmake_minimum_required(VERSION 3.29)

Project("ArcherEngine")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_FOUNDATION "Enable Foundation" ON)
if(ENABLE_FOUNDATION)
	add_subdirectory(Foundation)
endif()

option(ENABLE_TESTS "Set to OFF|ON (default is OFF) to control build of ArcherEngine tests & samples" ON)
