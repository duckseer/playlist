cmake_minimum_required(VERSION 3.31)
project(playlist LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)  # 关键修复

set(CMAKE_EXE_LINKER_FLAGS "-static-libstdc++ -static-libgcc")

include_directories(../include)
link_directories(../lib)

add_executable(${PROJECT_NAME} WIN32
        main.cpp
        ui/Window.cpp
        ui/Callbacks.cpp
        core/audio_file.cpp
        core/playlist.cpp
        utils/file_utils.cpp
)

target_link_libraries(${PROJECT_NAME}
        fltk fltk_images fltk_jpeg fltk_png fltk_z
        ws2_32 User32 comctl32 GdiPlus
)