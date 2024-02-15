cmake_minimum_required(VERSION 3.22)

# Explorer
project (stereo-explorer VERSION 0.1 DESCRIPTION "Stereo disparity explorer application.")

file(GLOB SRC_FILES "src/*.cpp")
add_executable(stereo-explorer ${SRC_FILES})
include_directories(stereo-explorer PUBLIC "incl")
target_link_libraries(stereo-explorer imgui png)

# ImGui
add_library(imgui STATIC imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/backends/imgui_impl_glfw.cpp imgui/backends/imgui_impl_opengl3.cpp)
target_link_libraries(imgui GL glfw)
target_include_directories(imgui PUBLIC "." "imgui")
