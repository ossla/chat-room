cmake_minimum_required(VERSION 3.14)
project(ImGuiExample)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

# --- GLFW ---
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.8
)
FetchContent_MakeAvailable(glfw)

# --- ImGui ---
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.9
)
FetchContent_MakeAvailable(imgui)

find_package(OpenGL REQUIRED)

# --- Создание исполняемого файла ---
add_executable(client
    client_entry.cpp

    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# --- Пути к заголовочным файлам ---
target_include_directories(client PRIVATE
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

# --- Линковка библиотек ---
target_link_libraries(client PRIVATE glfw OpenGL::GL)

# --- Для Windows ---
if (WIN32)
    target_compile_definitions(client PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
