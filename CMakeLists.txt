cmake_minimum_required(VERSION 3.1)

project(campoelettrico)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 -std=c++1y -Wall -Wextra")

add_executable(campoelettrico
main.cpp
timer.cpp
ui/ui.cpp
ui/imgui.cpp
ui/imgui_demo.cpp
ui/imgui_draw.cpp
ui/imgui_tables.cpp
ui/imgui_widgets.cpp
ui/imgui_impl_sdlrenderer.cpp
ui/imgui_impl_sdl.cpp
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
#PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
#PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf>=2.0.0)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS}) #${SDL2IMAGE_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} )#${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES})
