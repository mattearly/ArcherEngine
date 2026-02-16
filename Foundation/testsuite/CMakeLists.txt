cmake_minimum_required(VERSION 3.15)

add_executable(testsuite UnitTest.cpp)

target_link_libraries(testsuite PRIVATE archer)

target_include_directories(testsuite PRIVATE "${CMAKE_SOURCE_DIR}/Foundation/include")

if (UNIX)
    #put special unix commands here
elseif (WIN32)
	target_link_libraries(testsuite PRIVATE glm::glm imgui::imgui)
endif()

add_test(NAME defaultinit COMMAND testsuite)
