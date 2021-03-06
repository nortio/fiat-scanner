cmake_minimum_required(VERSION 3.1)

project(test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -g -O3 -Wall -Wextra")

add_executable(test
main.cpp
timer.cpp
interfaces.cpp
ui/ui.cpp
ui/imgui.cpp
ui/imgui_demo.cpp
ui/imgui_draw.cpp
ui/imgui_tables.cpp
ui/imgui_widgets.cpp
ui/imgui_impl_sdlrenderer.cpp
ui/imgui_impl_sdl.cpp
)

add_executable(testClient
testClient/test.cpp
)

set (CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
#PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
#PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf>=2.0.0)
target_link_libraries(test CppLinuxSerial)
target_link_libraries(testClient CppLinuxSerial)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS}) #${SDL2IMAGE_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} )#${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES})
