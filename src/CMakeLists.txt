cmake_minimum_required(VERSION 3.31)
project(playlist LANGUAGES CXX)
set(CMAKE_EXE_LINKER_FLAGS "-static-libstdc++ -static-libgcc")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

include_directories(../include)
link_directories(../lib)

file(GLOB SOURCES "*.cpp")
add_executable(${PROJECT_NAME} WIN32 ${SOURCES}
        main.cpp)

target_link_libraries(
        ${PROJECT_NAME}
        fltk
        fltk_images
        fltk_jpeg
        fltk_png
        fltk_z
        ws2_32
        User32
        comctl32
        GdiPlus
)
