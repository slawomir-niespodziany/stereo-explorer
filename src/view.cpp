#include "view.h"
#include <iomanip>
#include <iostream>
#include <stdexcept>

void View::loop(Model& model) {
    pModel_ = &model;

    userInit();
    while (!glfwWindowShouldClose(pWindow_)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        userFrame();

        glfwGetFramebufferSize(pWindow_, reinterpret_cast<int*>(&windowWidth_), reinterpret_cast<int*>(&windowHeight_));
        glViewport(0, 0, static_cast<int>(windowWidth_), static_cast<int>(windowHeight_));

        glClearColor(windowColor_.x, windowColor_.y, windowColor_.z, windowColor_.w);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(pWindow_);
    }
}

static void cbError(int error, const char* info) {
    std::cerr << "GLFW Error " << std::setw(3) << error << ": ";
    if (nullptr != info) {
        std::cerr << info;
    }
    std::cerr << std::endl;
}

View::View() {
    glfwSetErrorCallback(&cbError);

    if (GLFW_TRUE != glfwInit()) {
        throw std::runtime_error("glfwInit() failed."s);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    pWindow_ = glfwCreateWindow(windowWidth_, windowHeight_, windowTitle_.c_str(), nullptr, nullptr);
    if (nullptr == pWindow_) {
        throw std::runtime_error("glfwCreateWindow() failed."s);
    }

    glfwMakeContextCurrent(pWindow_);
    glfwSwapInterval(1);   // Enable vsync

    if (!IMGUI_CHECKVERSION()) {
        throw std::runtime_error("IMGUI_CHECKVERSION() failed."s);
    }

    ImGui::CreateContext();

    ImGuiIO& imGuiIo = ImGui::GetIO();
    imGuiIo.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    imGuiIo.IniFilename = NULL;

    ImGui::StyleColorsDark();

    if (!ImGui_ImplGlfw_InitForOpenGL(pWindow_, true)) {
        throw std::runtime_error("ImGui_ImplGlfw_InitForOpenGL() failed."s);
    }

    if (!ImGui_ImplOpenGL3_Init(nullptr)) {
        throw std::runtime_error("ImGui_ImplOpenGL3_Init() failed."s);
    }
}

void View::setImgBuffer(unsigned int handle, unsigned int height, unsigned int width, const PixelRgb* pData) {
    glBindTexture(GL_TEXTURE_2D, handle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pData);
}
