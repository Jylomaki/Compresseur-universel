cmake_minimum_required(VERSION 3.8)
project(COMPRESSEUR)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp ImageBase.h ImageBase.cpp image_ppm.h stb_image_write.h stb_image.h)
add_executable(Gitan ${SOURCE_FILES})