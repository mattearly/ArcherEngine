add_executable(fpp_demo main.cpp "../common/scripts/fpp_fly.h")

target_link_libraries(fpp_demo PRIVATE ${CMAKE_PROJECT_NAME})

target_include_directories(fpp_demo PRIVATE "${CMAKE_SOURCE_DIR}/examples/common/scripts" "${CMAKE_SOURCE_DIR}/include")

if (UNIX) 

elseif (WIN32)
	target_include_directories(fpp_demo PRIVATE "${CMAKE_SOURCE_DIR}/examples/common/scripts" "${CMAKE_SOURCE_DIR}/include")
	target_link_libraries(fpp_demo PRIVATE glm::glm)
endif()
