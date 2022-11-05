add_executable(editor_example main.cpp "../common/scripts/fpp_fly.h" "../common/scripts/file_menu.h")

target_link_libraries(editor_example PRIVATE archer)

target_include_directories(editor_example PRIVATE "${CMAKE_SOURCE_DIR}/examples/common/scripts" "${CMAKE_SOURCE_DIR}/include")

if (UNIX) 

elseif (WIN32)
	target_include_directories(editor_example PRIVATE "${CMAKE_SOURCE_DIR}/examples/common/scripts" "${CMAKE_SOURCE_DIR}/include")
	target_link_libraries(editor_example PRIVATE glm::glm imgui::imgui)
endif()