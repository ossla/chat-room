#pragma once
#include "../chat-room.hpp"
#include "pool_messages.cpp"
#include "client.cpp"
#include <iostream> 
#include <mutex> 
#include <memory> 

// пример кода imgui/examples/example_glfw_opengl3/main.cpp

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h> // Will drag system OpenGL headers


#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif


namespace UI {

static void glfw_error_callback(int error, const char* description)
{
   std::cerr << "GLFW Error "<< error << ": " << description;
}

/* описание логики вывода сообщений */
class ChatRoomUI {
private:
    PoolMessages pool_messages_;
    std::mutex pool_mutex_;

    std::shared_ptr<ChatClient> client_;
public:
    void BindChatRoom(std::shared_ptr<ChatClient> client) {
        client_ = client;
        client_->on_message_received = [this](std::string msg) {
            this->AddMessage(msg.data(), false);
        };
    }

    inline void ShowMainPart(char input_buff[]) {
        ImVec2 size(500, 700);
        ImGui::SetNextWindowSize(size);
        {
            ImGui::Begin("chat-room");
            {
                std::lock_guard<std::mutex> lock(pool_mutex_);
                for (const auto& message : pool_messages_) {
                    ImGui::Text("%s\n", message.c_str());
                }
            }

            ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 30);
            if (ImGui::InputText("##input", input_buff, PoolMessages::MAX_LEN_MSG
                                , ImGuiInputTextFlags_EnterReturnsTrue)) {
                AddMessage(input_buff, true);
            }
            ImGui::SameLine();
            if (ImGui::Button("Send")) {
                AddMessage(input_buff, true);
            }
            ImGui::End();
        }
    }

private:
    void AddMessage(char input_buff[], bool is_you) {
        size_t size_msg = strnlen(input_buff, PoolMessages::MAX_LEN_MSG);
        if (is_you) {
            client_->SendMessage(input_buff, size_msg);
        }
        std::lock_guard<std::mutex> lock(pool_mutex_);
        pool_messages_.AddMessage(input_buff, size_msg, is_you);
    }

public:
    void RenderMainThread(GLFWwindow* window) {
        client_->StartRecvMessages();

        char input_buff[PoolMessages::MAX_LEN_MSG];
        memset(input_buff, 0, sizeof(input_buff));

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
            {
                ImGui_ImplGlfw_Sleep(10);
                continue;
            }

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ShowMainPart(input_buff);

            // Rendering
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }
    }
};


void RenderUI(std::shared_ptr<ChatClient> client) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        throw UIException("glfw initialization problem");

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    const char* glsl_version = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr)
        throw UIException("glfw create window problem");
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

/*=========================================================== */
    ChatRoomUI room_ui;
    room_ui.BindChatRoom(client);
    room_ui.RenderMainThread(window);
/*===========================================================*/

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

}
} // namespace UI