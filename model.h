#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include "pixel.h"

class Model {
public:
    Model(const std::string &imgPathLeft, const std::string &imgPathRight, const std::string &pathDepthTrue, uint16_t blockSize, uint16_t maxDisp);
    virtual ~Model() {
        delete[] pImgLeftRgb_;
        delete[] pImgRightRgb_;
        delete[] pImgLeftGray_;
        delete[] pImgRightGray_;

        delete[] pDepthTrue_;
        delete[] pDepthCalc_;
        delete[] pDepthDiff_;

        delete[] pImgDepthTrue_;
        delete[] pImgDepthCalc_;
        delete[] pImgDepthDiff_;

        delete[] pCost_;
        delete[] pAggr_;
    }

    unsigned int getWidth() { return width_; }
    unsigned int getHeight() { return height_; }
    unsigned int getBlockSize() { return blockSize_; }
    unsigned int getMaxDisp() { return maxDisp_; }

    const PixelRgb *getImgLeftRgb() { return pImgLeftRgb_; }
    const PixelRgb *getImgRightRgb() { return pImgRightRgb_; }

    const PixelGray *getImgLeftGray() { return pImgLeftGray_; }
    const PixelGray *getImgRightGray() { return pImgRightGray_; }

    const float *getDepthTrue() { return pDepthTrue_; }
    const float *getDepthCalc() { return pDepthCalc_; }
    const float *getDepthDiff() { return pDepthDiff_; }

    const PixelRgb *getImgDepthTrue() { return pImgDepthTrue_; }
    const PixelRgb *getImgDepthCalc() { return pImgDepthCalc_; }
    const PixelRgb *getImgDepthDiff() { return pImgDepthDiff_; }

private:
    void loadImgRgb(const std::string &pathLeft, const std::string &pathRight);

    template <typename Dst, typename Src>
    Dst *convertTo(const Src *pSrc);

    float *loadPfm(const std::string &path);

    template <typename Pixel>
    uint32_t *costCalc(const Pixel *pLeft, const Pixel *pRight);
    uint32_t *costAggr();
    uint32_t *costForPixel(uint32_t x, uint32_t y);
    float *costMin();

    const uint16_t blockSize_, maxDisp_, nDisp_;

    uint32_t height_, width_, size_;
    PixelRgb *pImgLeftRgb_, *pImgRightRgb_;
    PixelGray *pImgLeftGray_, *pImgRightGray_;

    float *pDepthTrue_;
    PixelRgb *pImgDepthTrue_;

    uint32_t *pCost_ /* HEIGHT * WIDTH * NDISP */;
    uint32_t *pAggr_ /* HEIGHT * WIDTH * NDISP */;

    float *pDepthCalc_, *pDepthDiff_;
    PixelRgb *pImgDepthCalc_, *pImgDepthDiff_;
};
