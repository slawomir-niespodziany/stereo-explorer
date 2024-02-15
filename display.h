#pragma once

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <utility>
#include "pixel.h"

class Display {
public:
    Display(uint32_t height, uint32_t width, uint32_t maxDisp)
        : displayHeight_(height),
          displayWidth_(width),
          displayMidY_(displayHeight_ / 2),
          displayMidX_((displayWidth_ + maxDisp + 1) / 2),
          pDisplayLeft_(new PixelRgb[displayHeight_ * displayWidth_]),
          pDisplayRight_(new PixelRgb[displayHeight_ * displayWidth_]) {}

    virtual ~Display() {
        delete[] pDisplayLeft_;
        delete[] pDisplayRight_;
    }

    bool setImage(uint32_t height, uint32_t width, const PixelRgb* pImgLeft, const PixelRgb* pImgRight) {
        bool renderLeft{false}, renderRight{false};

        if ((height != imgHeight_) || (width != imgWidth_)) {
            imgHeight_ = height;
            imgWidth_ = width;

            focusX_ = imgWidth_ / 2;
            focusY_ = imgHeight_ / 2;

            renderLeft = renderRight = true;
        }

        if (pImgLeft != pImgLeft_) {
            pImgLeft_ = pImgLeft;
            renderLeft = true;
        }

        if (pImgRight != pImgRight_) {
            pImgRight_ = pImgRight;
            renderRight = true;
        }

        render(renderLeft, renderRight);
        return (renderLeft || renderRight);
    }

    bool translateFocus(int32_t dx, int32_t dy) {
        uint32_t focusX = std::min(imgWidth_ - 1u, static_cast<uint32_t>(std::max(0, (static_cast<int32_t>(focusX_) + dx))));
        uint32_t focusY = std::min(imgHeight_ - 1u, static_cast<uint32_t>(std::max(0, (static_cast<int32_t>(focusY_) + dy))));

        if ((focusX != focusX_) || (focusY != focusY_)) {
            focusX_ = focusX;
            focusY_ = focusY;

            render(true, true);
            return true;
        }

        return false;
    }

    uint32_t getHeight() { return displayHeight_; }
    uint32_t getWidth() { return displayWidth_; }
    uint32_t getMidX() { return displayMidX_; }
    uint32_t getMidY() { return displayMidY_; }
    const PixelRgb* getLeft() { return pDisplayLeft_; }
    const PixelRgb* getRight() { return pDisplayRight_; }
    uint32_t getFocusX() { return focusX_; }
    uint32_t getFocusY() { return focusY_; }

private:
    void render(bool left, bool right) {
        const auto render = [this](const PixelRgb* pSrc, PixelRgb* pDst) {
            int32_t ySrc = static_cast<int32_t>(focusY_) - static_cast<int32_t>(displayMidY_), ySrcEnd = ySrc + displayHeight_,
                    ySrcBreak = std::min(ySrcEnd, static_cast<int32_t>(imgHeight_));
            for (; ySrc < 0; ySrc++) {
                for (int32_t x = 0; x < displayWidth_; x++) {
                    *(pDst++) = {0u, 0u, 0u};
                }
            }

            int32_t xSrcBegin = static_cast<int32_t>(focusX_) - static_cast<int32_t>(displayMidX_), xSrcEnd = xSrcBegin + displayWidth_,
                    xSrcBreak = std::min(xSrcEnd, static_cast<int32_t>(imgWidth_));
            for (; ySrc < ySrcBreak; ySrc++) {
                int32_t xSrc = xSrcBegin;
                const PixelRgb* pSrc_ = pSrc + (ySrc * imgWidth_);

                for (; xSrc < 0; xSrc++) {
                    *(pDst++) = {0u, 0u, 0u};
                }
                for (; xSrc < xSrcBreak; xSrc++) {
                    *(pDst++) = *(pSrc_ + xSrc);
                }
                for (; xSrc < xSrcEnd; xSrc++) {
                    *(pDst++) = {0u, 0u, 0u};
                }
            }

            for (; ySrc < ySrcEnd; ySrc++) {
                for (int32_t x = 0; x < displayWidth_; x++) {
                    *(pDst++) = {0u, 0u, 0u};
                }
            }
        };

        if (left) {
            render(pImgLeft_, pDisplayLeft_);
        }
        if (right) {
            render(pImgRight_, pDisplayRight_);
        }
    }

    const uint32_t displayHeight_, displayWidth_;
    const uint32_t displayMidX_, displayMidY_;
    PixelRgb *const pDisplayLeft_, *const pDisplayRight_;

    uint32_t imgHeight_, imgWidth_;
    const PixelRgb *pImgLeft_, *pImgRight_;

    uint32_t focusX_, focusY_;
};