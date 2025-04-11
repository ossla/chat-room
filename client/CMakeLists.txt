cmake_minimum_required(VERSION 3.10)
project(ImGuiExample)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# исполняемый файл
add_executable(client client_entry.cpp
    ./imgui/imgui_demo.cpp # для разработки
    ./imgui/imgui.cpp 
    ./imgui/imgui_draw.cpp 
    ./imgui/imgui_widgets.cpp
    ./imgui/imgui_tables.cpp
    ./imgui/backends/imgui_impl_glfw.cpp 
    ./imgui/backends/imgui_impl_opengl3.cpp)

# пути к заголовочным файлам
target_include_directories(client PRIVATE ./imgui/ ./imgui/backends)

# линковка либ
target_link_libraries(client PRIVATE glfw OpenGL::GL)
