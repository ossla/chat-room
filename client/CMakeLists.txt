cmake_minimum_required(VERSION 3.14) # FetchContent стабилен с 3.14+
project(ImGuiExample)

set(CMAKE_CXX_STANDARD 17)

# --- FetchContent ---
include(FetchContent)

# GLFW (будет скачан автоматически)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.8 # или latest stable
)
FetchContent_MakeAvailable(glfw)

# --- OpenGL ---
find_package(OpenGL REQUIRED)

# --- Имя исполняемого файла и исходники ---
add_executable(client
    client_entry.cpp

    # ImGui
    ./imgui/imgui.cpp
    ./imgui/imgui_demo.cpp
    ./imgui/imgui_draw.cpp
    ./imgui/imgui_widgets.cpp
    ./imgui/imgui_tables.cpp

    # ImGui backends
    ./imgui/backends/imgui_impl_glfw.cpp
    ./imgui/backends/imgui_impl_opengl3.cpp
)

# --- Пути к заголовочным файлам ---
target_include_directories(client PRIVATE
    ./imgui
    ./imgui/backends
    ${glfw_SOURCE_DIR}/include
)

# --- Линковка ---cmake_minimum_required(VERSION 3.10)
project(ImGuiExample)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# исполняемый файл
add_executable(client client_entry.cpp
    ./imgui/imgui_demo.cpp # для разработки
    ./imgui/imgui.cpp 
    ./imgui/imgui_draw.cpp cmake_minimum_required(VERSION 3.10)
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
    
    ./imgui/imgui_widgets.cpp
    ./imgui/imgui_tables.cpp
    ./imgui/backends/imgui_impl_glfw.cpp 
    ./imgui/backends/imgui_impl_opengl3.cpp)

# пути к заголовочным файлам
target_include_directories(client PRIVATE ./imgui/ ./imgui/backends)

# линковка либ
target_link_libraries(client PRIVATE glfw OpenGL::GL)

target_link_libraries(client PRIVATE glfw OpenGL::GL)

# --- Windows специфические настройки ---
if (WIN32)
    target_compile_definitions(client PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
