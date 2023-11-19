cmake_minimum_required(VERSION 3.0)
project(ssTools-example)

aux_source_directory(${CMAKE_SOURCE_DIR}/ssTools CPPS)
message ("cpps " ${CPPS})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
add_compile_options(-g)
add_compile_options(-fpermissive)

add_executable(example ${CPPS} ${CMAKE_SOURCE_DIR}/Example/main.cpp)

if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Windows")
    message("os: windows")
    target_link_libraries(example)
else()
    message("os: linux")
    target_link_libraries(example pthread)
endif()