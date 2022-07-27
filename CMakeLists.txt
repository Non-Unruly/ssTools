project(ssToolsDemo)
cmake_minimum_required(VERSION 3.0)

aux_source_directory(${CMAKE_SOURCE_DIR}/src CPPS)
message ("src cpps " ${CPPS})

add_compile_options(-g)

add_executable(out ${CPPS} ${CMAKE_SOURCE_DIR}/Example/main.cpp )
target_link_libraries(out pthread)