cmake_minimum_required(VERSION 3.8)
project(onster)

find_package(SDL2 REQUIRED)

add_executable(onster src/linux_onster.c)

target_link_libraries(onster PUBLIC ${SDL2_LIBRARIES})
target_include_directories(onster PUBLIC src/ ${SDL2_INCLUDE_DIRS})

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
target_compile_definitions(onster PUBLIC -DATOM_DEBUG=1)
endif()
