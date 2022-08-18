add_executable(fpp_demo main.cpp)

target_link_libraries(fpp_demo PRIVATE archer)
target_include_directories(fpp_demo PRIVATE "${CMAKE_SOURCE_DIR}/examples/common/scripts" "${CMAKE_SOURCE_DIR}/include")
