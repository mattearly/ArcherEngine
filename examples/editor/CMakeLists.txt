file(GLOB SCRIPTS ../common/scripts/*.h)

add_executable(editor_example main.cpp ${SCRIPTS})

target_link_libraries(editor_example PRIVATE archer)

target_include_directories(editor_example 
  PRIVATE 
    "${CMAKE_SOURCE_DIR}/examples/common/scripts"
    "${CMAKE_SOURCE_DIR}/include")

if (UNIX) 
    #put special unix commands here
elseif (WIN32)
	target_link_libraries(editor_example PRIVATE glm::glm imgui::imgui)
endif()
