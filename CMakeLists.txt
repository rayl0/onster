cmake_minimum_required(VERSION 3.8)
project(onster)

find_package(SDL2 REQUIRED)

add_library(glad STATIC src/glad/src/glad.c)

target_include_directories(glad PUBLIC src/glad/include)
target_link_libraries(glad PUBLIC GL dl)

add_library(stb_image STATIC src/stb_image/stb_image.c)
target_include_directories(stb_image PUBLIC src/stb_image)

add_library(onsterlib src/onster.cc)
add_executable(onster src/linux_onster.cc)

target_link_libraries(onster PUBLIC ${SDL2_LIBRARIES} stb_image glad onsterlib)
target_include_directories(onster PUBLIC src/ ${SDL2_INCLUDE_DIRS})

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
  target_compile_options(onster PUBLIC -DONSTER_DEBUG=1
    -Wall -Wextra)
endif()
