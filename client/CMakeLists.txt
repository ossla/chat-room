cmake_minimum_required(VERSION 3.14)
project(ImGuiExample)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

# --- Fetch GLFW from source ---
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.8
)
FetchContent_MakeAvailable(glfw)

# --- OpenGL ---
find_package(OpenGL REQUIRED)

# --- Executable and source files ---
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

# --- Include paths ---
target_include_directories(client PRIVATE
    ./imgui
    ./imgui/backends
)

# --- Link libraries ---
target_link_libraries(client PRIVATE glfw OpenGL::GL)

if (WIN32)
    target_compile_definitions(client PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
