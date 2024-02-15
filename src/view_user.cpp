#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include "view.h"

void View::userInit() {
    const uint32_t displayWidth = 800u, displayHeight = 600u;

    pDisplayLeft_ = pModel_->getImgLeftRgb();
    pDisplayRight_ = pModel_->getImgRightRgb();

    pDisplay_ = new Display(displayHeight, displayWidth, pModel_->getMaxDisp());
    pDisplay_->setImage(pModel_->getHeight(), pModel_->getWidth(), pDisplayLeft_, pDisplayRight_);

    initImgHandle(hDisplayLeft);
    initImgHandle(hDisplayRight);

    refreshDisplay();
}

void View::drawHeaderGap() {
    ImGui::BeginChild("##Child_HeaderGap", ImVec2(32, 0));
    ImGui::EndChild();
}

void View::drawLeftDisplayHeader() {
    ImGui::BeginChild("##Child_LeftDisplayHeader", ImVec2(pDisplay_->getWidth(), 0));

    if (ImGui::Button("RGB", ImVec2(64, 32))) {
        pDisplayLeft_ = pModel_->getImgLeftRgb();

        if (pDisplay_->setImage(pModel_->getHeight(), pModel_->getWidth(), pDisplayLeft_, pDisplayRight_)) {
            refreshDisplay();
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Gray", ImVec2(64, 32))) {
        pDisplayLeft_ = reinterpret_cast<const PixelRgb *>(pModel_->getImgLeftGray());

        if (pDisplay_->setImage(pModel_->getHeight(), pModel_->getWidth(), pDisplayLeft_, pDisplayRight_)) {
            refreshDisplay();
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("d-Diff", ImVec2(64, 32))) {
        pDisplayLeft_ = reinterpret_cast<const PixelRgb *>(pModel_->getImgDepthDiff());

        if (pDisplay_->setImage(pModel_->getHeight(), pModel_->getWidth(), pDisplayLeft_, pDisplayRight_)) {
            refreshDisplay();
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("d-Calc", ImVec2(64, 32))) {
        pDisplayLeft_ = reinterpret_cast<const PixelRgb *>(pModel_->getImgDepthCalc());

        if (pDisplay_->setImage(pModel_->getHeight(), pModel_->getWidth(), pDisplayLeft_, pDisplayRight_)) {
            refreshDisplay();
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("d-True", ImVec2(64, 32))) {
        pDisplayLeft_ = reinterpret_cast<const PixelRgb *>(pModel_->getImgDepthTrue());

        if (pDisplay_->setImage(pModel_->getHeight(), pModel_->getWidth(), pDisplayLeft_, pDisplayRight_)) {
            refreshDisplay();
        }
    }

    ImGui::EndChild();
}
void View::drawRightDisplayHeader() {
    ImGui::BeginChild("##Child_RightDisplayHeader", ImVec2(pDisplay_->getWidth(), 0));

    if (ImGui::Button("RGB", ImVec2(64, 32))) {
        pDisplayRight_ = pModel_->getImgRightRgb();

        if (pDisplay_->setImage(pModel_->getHeight(), pModel_->getWidth(), pDisplayLeft_, pDisplayRight_)) {
            refreshDisplay();
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Gray", ImVec2(64, 32))) {
        pDisplayRight_ = reinterpret_cast<const PixelRgb *>(pModel_->getImgRightGray());

        if (pDisplay_->setImage(pModel_->getHeight(), pModel_->getWidth(), pDisplayLeft_, pDisplayRight_)) {
            refreshDisplay();
        }
    }

    ImGui::EndChild();
}

void View::drawVerticalBar() {
    ImGui::BeginChild("##Child_VerticalBar", ImVec2(32, 0));

    if (ImGui::Button("-10", ImVec2(32, 32)) && (pDisplay_->translateFocus(0, -10))) {
        refreshDisplay();
    }
    if (ImGui::Button("-1", ImVec2(32, 32)) && (pDisplay_->translateFocus(0, -1))) {
        refreshDisplay();
    }

    uint32_t min = 0, max = pModel_->getHeight() - 1, curr = pDisplay_->getFocusY(), val = max - curr;
    ImGui::SetCursorPosX(7.5f);
    if (ImGui::VSliderScalar("##RowSelectorVSlider", ImVec2(19, pDisplay_->getHeight() - 180), ImGuiDataType_U32, &val, &min, &max, "") &&
        (pDisplay_->translateFocus(0, (max - val) - curr))) {
        refreshDisplay();
    }

    if (ImGui::Button("+1", ImVec2(32, 32)) && (pDisplay_->translateFocus(0, 1))) {
        refreshDisplay();
    }

    if (ImGui::Button("+10", ImVec2(32, 32)) && (pDisplay_->translateFocus(0, 10))) {
        refreshDisplay();
    }

    ImGui::Text("y:  \n%u", (max - val));

    ImGui::EndChild();
}

void View::drawLeftDisplay() {
    const ImVec2 size(pDisplay_->getWidth(), pDisplay_->getHeight());
    const ImVec2 mid(pDisplay_->getMidX(), pDisplay_->getMidY());

    ImGui::BeginChild("##Child_LeftDisplay", size);

    const ImVec2 zero = ImGui::GetCursorScreenPos();

    ImGui::Image(reinterpret_cast<ImTextureID>(hDisplayLeft), size);

    if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left) &&
        (pDisplay_->translateFocus(cursorPosLast_.x - cursorPosCurr_.x, cursorPosLast_.y - cursorPosCurr_.y))) {
        refreshDisplay();
    }

    const uint32_t r = pModel_->getBlockSize() / 2u, R = 1 + r;
    const ImVec2 guideN[]{{zero.x + mid.x - R - r, zero.y + mid.y - R}, {zero.x + mid.x + R + r + 1, zero.y + mid.y - R}};
    const ImVec2 guideS[]{{zero.x + mid.x - R - r, zero.y + mid.y + R}, {zero.x + mid.x + R + r + 1, zero.y + mid.y + R}};
    const ImVec2 guideW[]{{zero.x + mid.x - R, zero.y + mid.y - R - r}, {zero.x + mid.x - R, zero.y + mid.y + R + r + 1}};
    const ImVec2 guideE[]{{zero.x + mid.x + R, zero.y + mid.y - R - r}, {zero.x + mid.x + R, zero.y + mid.y + R + r + 1}};
    const ImU32 guideColor[]{ImGui::GetColorU32({1.0f, 1.0f, 1.0f, 0.75f}), ImGui::GetColorU32({0.0f, 0.0f, 0.0f, 0.75f})};

    ImGui::GetWindowDrawList()->AddLine(guideN[0], guideN[1], guideColor[0]);
    ImGui::GetWindowDrawList()->AddLine(guideS[0], guideS[1], guideColor[1]);
    ImGui::GetWindowDrawList()->AddLine(guideW[0], guideW[1], guideColor[0]);
    ImGui::GetWindowDrawList()->AddLine(guideE[0], guideE[1], guideColor[1]);

    ImGui::EndChild();
}

void View::drawRightDisplay() {
    const ImVec2 size(pDisplay_->getWidth(), pDisplay_->getHeight());
    const ImVec2 mid(pDisplay_->getMidX(), pDisplay_->getMidY());

    ImGui::BeginChild("##Child_RightDisplay", size);

    const ImVec2 zero = ImGui::GetCursorScreenPos();

    ImGui::Image(reinterpret_cast<ImTextureID>(hDisplayRight), size);

    if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        currDisp_ = std::min(pModel_->getMaxDisp(), static_cast<uint32_t>(std::max(0, static_cast<int32_t>(pDisplay_->getMidX() - (cursorPosCurr_.x - zero.x)))));
    }

    const uint32_t r = pModel_->getBlockSize() / 2u, R = 1 + r;

    const ImVec2 guideN[]{{zero.x + mid.x - R - r, zero.y + mid.y - R}, {zero.x + mid.x + R + r + 1, zero.y + mid.y - R}};
    const ImVec2 guideS[]{{zero.x + mid.x - R - r, zero.y + mid.y + R}, {zero.x + mid.x + R + r + 1, zero.y + mid.y + R}};
    const ImVec2 guideW[]{{zero.x + mid.x - R, zero.y + mid.y - R - r}, {zero.x + mid.x - R, zero.y + mid.y + R + r + 1}};
    const ImVec2 guideE[]{{zero.x + mid.x + R, zero.y + mid.y - R - r}, {zero.x + mid.x + R, zero.y + mid.y + R + r + 1}};
    const ImU32 guideColor{ImGui::GetColorU32({0.5f, 0.5f, 0.5f, 0.75f})};

    ImGui::GetWindowDrawList()->AddLine(guideN[0], guideN[1], guideColor);
    ImGui::GetWindowDrawList()->AddLine(guideS[0], guideS[1], guideColor);
    ImGui::GetWindowDrawList()->AddLine(guideW[0], guideW[1], guideColor);
    ImGui::GetWindowDrawList()->AddLine(guideE[0], guideE[1], guideColor);

    const ImVec2 dispN[]{{guideN[0].x - currDisp_, guideN[0].y}, {guideN[1].x - currDisp_, guideN[1].y}};
    const ImVec2 dispS[]{{guideS[0].x - currDisp_, guideS[0].y}, {guideS[1].x - currDisp_, guideS[1].y}};
    const ImVec2 dispW[]{{guideW[0].x - currDisp_, guideW[0].y}, {guideW[1].x - currDisp_, guideW[1].y}};
    const ImVec2 dispE[]{{guideE[0].x - currDisp_, guideE[0].y}, {guideE[1].x - currDisp_, guideE[1].y}};
    const ImU32 dispColor[]{ImGui::GetColorU32({1.0f, 1.0f, 1.0f, 0.75f}), ImGui::GetColorU32({0.0f, 0.0f, 0.0f, 0.75f})};

    ImGui::GetWindowDrawList()->AddLine(dispN[0], dispN[1], dispColor[0]);
    ImGui::GetWindowDrawList()->AddLine(dispS[0], dispS[1], dispColor[1]);
    ImGui::GetWindowDrawList()->AddLine(dispW[0], dispW[1], dispColor[0]);
    ImGui::GetWindowDrawList()->AddLine(dispE[0], dispE[1], dispColor[1]);

    ImGui::EndChild();
}

void View::drawFooterGap() {
    ImGui::BeginChild("##Child_FooterGap", ImVec2(32, 0));
    ImGui::EndChild();
}

void View::drawLeftDisplayFooter() {
    ImGui::BeginChild("##Child_LeftDisplayFooter", ImVec2(pDisplay_->getWidth(), 0));

    uint32_t min = 0, max = pModel_->getWidth() - 1, curr = pDisplay_->getFocusX(), val = curr;
    ImGui::Text("y:  \n%u", val);

    ImGui::SameLine();
    if (ImGui::Button("-10", ImVec2(32, 32)) && (pDisplay_->translateFocus(-10, 0))) {
        refreshDisplay();
    }

    ImGui::SameLine();
    if (ImGui::Button("-1", ImVec2(32, 32)) && (pDisplay_->translateFocus(-1, 0))) {
        refreshDisplay();
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(pDisplay_->getWidth() - 196);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6.5f);
    if (ImGui::SliderScalar("##ColSelectorSlider", ImGuiDataType_U32, &val, &min, &max, "") && (pDisplay_->translateFocus(val - curr, 0))) {   // FIXME TO ABSOLUTE (not dx/dy)
        refreshDisplay();
    }

    ImGui::SameLine();
    if (ImGui::Button("+1", ImVec2(32, 32)) && (pDisplay_->translateFocus(1, 0))) {
        refreshDisplay();
    }

    ImGui::SameLine();
    if (ImGui::Button("+10", ImVec2(32, 32)) && (pDisplay_->translateFocus(10, 0))) {
        refreshDisplay();
    }

    ImGui::EndChild();
}

void View::drawRightDisplayFooter() {
    uint32_t min = 0, max = pModel_->getMaxDisp(), val;

    ImGui::BeginChild("##Child_RightDisplayFooter", ImVec2(pDisplay_->getWidth(), 0));

    ImGui::Text("d:  \n%u", currDisp_);

    ImGui::SameLine();
    if (ImGui::Button("+10", ImVec2(32, 32))) {
        currDisp_ = std::min(max, currDisp_ + 10u);
    }

    ImGui::SameLine();
    if (ImGui::Button("+1", ImVec2(32, 32))) {
        currDisp_ = std::min(max, currDisp_ + 1u);
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(pDisplay_->getWidth() - 196);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6.5f);

    val = max - currDisp_;
    if (ImGui::SliderScalar("##DispSelectorSlider", ImGuiDataType_U32, &val, &min, &max, "")) {
        currDisp_ = max - val;
    }

    ImGui::SameLine();
    if (ImGui::Button("-1", ImVec2(32, 32))) {
        currDisp_ = std::max(0, static_cast<int32_t>(currDisp_) - 1);
    }

    ImGui::SameLine();
    if (ImGui::Button("-10", ImVec2(32, 32))) {
        currDisp_ = std::max(0, static_cast<int32_t>(currDisp_) - 10);
    }

    ImGui::EndChild();
}

void View::drawDispPlotGap0() {
    ImGui::BeginChild("##Child_DispPlotGap0", ImVec2(32, 0));
    ImGui::EndChild();
}

void View::drawDispPlotGap1() {
    ImGui::BeginChild("##Child_DispPlotGap1", ImVec2(pDisplay_->getWidth(), 0));
    ImGui::EndChild();
}

void View::drawDispPlot() {
    ImGui::BeginChild("##Child_DispPlot", ImVec2(pDisplay_->getWidth(), 0));
    ImGui::Text("%u\n%u\n    ", currDisp_, 12);

    uint32_t n = pModel_->getMaxDisp() + 1;

    static float *pData = new float[n];
    static bool b = false;
    if (!b) {
        for (uint32_t i = 0; i < n; i++) {
            pData[i] = ((float) rand()) / RAND_MAX;
        }
        b = true;
    }

    ImGui::SameLine();
    ImGui::PlotLines("##DispPlot", pData, n, 0, 0, 0, 1, ImVec2(pDisplay_->getWidth(), 64));

    ImGui::EndChild();
}

void View::userFrame() {
    ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

    ImGui::Begin("Main window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    cursorPosCurr_ = ImGui::GetMousePos();

    ImGui::BeginChild("##Child_Row0", ImVec2(0, 32));
    {
        // Col 0
        drawHeaderGap();

        // Col 1: Left display header
        ImGui::SameLine();
        drawLeftDisplayHeader();

        // Col 2: Right display header
        ImGui::SameLine();
        drawRightDisplayHeader();
    }
    ImGui::EndChild();

    // Row 1
    ImGui::BeginChild("##Child_Row1", ImVec2(0, pDisplay_->getHeight()));
    {
        // Col 0: Vertical bar
        drawVerticalBar();

        // Col 1: Left display
        ImGui::SameLine();
        drawLeftDisplay();

        // Col 2: Right display
        ImGui::SameLine();
        drawRightDisplay();
    }
    ImGui::EndChild();

    // Row 2
    ImGui::BeginChild("##Child_Row2", ImVec2(0, 32));
    {
        // Col 0
        drawFooterGap();

        // Col 1: Left display footer
        ImGui::SameLine();
        drawLeftDisplayFooter();

        // Col 2: Right display footer
        ImGui::SameLine();
        drawRightDisplayFooter();
    }
    ImGui::EndChild();

    // Row 3
    ImGui::BeginChild("##Child_Row3", ImVec2(0, 128));
    {
        // Col 0
        drawDispPlotGap0();

        // Col 1
        ImGui::SameLine();
        drawDispPlotGap1();

        // Col 2: Disparity plot
        ImGui::SameLine();
        drawDispPlot();
    }
    ImGui::EndChild();

    cursorPosLast_ = cursorPosCurr_;
    ImGui::End();

    // ImGui::ShowDemoWindow();

    if (ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
        exit(0);
    }

    ImGui::Render();
}
