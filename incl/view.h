#pragma once

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>
#include <memory>
#include <string>
#include "display.h"
#include "model.h"
#include "pixel.h"

#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

using namespace std::string_literals;

class View {
public:
    virtual ~View() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();

        ImGui::DestroyContext();

        if (nullptr != pWindow_) {
            glfwDestroyWindow(pWindow_);
        }

        glfwTerminate();

        delete pDisplay_;
    }

    static View& getInstance() {
        static View instance;
        return instance;
    }

    void loop(Model& model);

private:
    View();

    void initImgHandle(unsigned int& handle) { glGenTextures(1, &handle); }
    void setImgBuffer(uint32_t handle, uint32_t height, uint32_t width, const PixelRgb* pData);
    void refreshDisplay() {
        setImgBuffer(hDisplayLeft, pDisplay_->getHeight(), pDisplay_->getWidth(), pDisplay_->getLeft());
        setImgBuffer(hDisplayRight, pDisplay_->getHeight(), pDisplay_->getWidth(), pDisplay_->getRight());
    }

    void userInit();
    void userFrame();

    void drawHeaderGap();
    void drawLeftDisplayHeader();
    void drawRightDisplayHeader();

    void drawVerticalBar();
    void drawLeftDisplay();
    void drawRightDisplay();

    void drawFooterGap();
    void drawLeftDisplayFooter();
    void drawRightDisplayFooter();

    void drawDispPlotGap0();
    void drawDispPlotGap1();
    void drawDispPlot();

    // Application window
    const std::string windowTitle_{"Stereo explorer"s};
    uint32_t windowWidth_{1800u}, windowHeight_{800u};
    GLFWwindow* pWindow_;
    const ImVec4 windowColor_{0.45f, 0.55f, 0.60f, 1.00f};

    // User data
    Model* pModel_;   // not owned by this class

    // Displays
    const PixelRgb *pDisplayLeft_, *pDisplayRight_;
    uint32_t hDisplayLeft, hDisplayRight;   // handles used for each display
    Display* pDisplay_;

    ImVec2 cursorPosCurr_, cursorPosLast_;

    uint32_t currDisp_;
};
