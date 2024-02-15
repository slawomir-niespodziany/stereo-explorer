#include "model.h"
#include <algorithm>
#include <bit>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <png++/png.hpp>
#include <stdexcept>

using namespace std::string_literals;

// Model::Model(const std::string &pathImgLeft, const std::string &pathImgRight, const std::string &pathDepthTrue, uint16_t blockSize, uint16_t maxDisp)
//     : blockSize_(blockSize), maxDisp_(maxDisp), nDisp_(maxDisp_ + 1) {
//     if ((0 == (blockSize % 2)) || (3 > blockSize)) {
//         throw std::runtime_error("Block size must be odd and greater than 2."s);
//     }

Model::Model(const std::string &pathImgLeft, const std::string &pathImgRight, const std::string &pathDepthTrue, uint16_t blockSize, uint16_t maxDisp)
    : blockSize_(blockSize), maxDisp_(maxDisp), nDisp_(maxDisp_ + 1) {
    // : blockSize_(3), maxDisp_(1), nDisp_(maxDisp_ + 1) {
    if ((0 == (blockSize % 2)) || (3 > blockSize)) {
        throw std::runtime_error("Block size must be odd and greater than 2."s);
    }

    // height_ = 4;
    // width_ = 5;

    // int8_t left[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    // int8_t right[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    // // pCost_ = costCalc(&left[0], &right[0]);
    // pCost_ = costCalc(&right[0], &left[0]);
    // pAggr_ = costAggr();
    // pDepthCalc_ = costMin();

    // for (uint32_t d = 0; d < nDisp_; d++) {
    //     for (uint32_t y = 0; y < height_; y++) {
    //         std::cout << "[ ";
    //         for (uint32_t x = 0; x < width_; x++) {
    //             // std::cout << std::setw(4) << *(pCost_ + (y * width_ * nDisp_) + (x * nDisp_) + d) << ' ';
    //             std::cout << std::setw(4) << *(pAggr_ + (y * width_ * nDisp_) + (x * nDisp_) + d) << ' ';
    //         }
    //         std::cout << "]\n";
    //     }
    //     std::cout << std::endl;
    // }

    // for (uint32_t y = 0; y < height_; y++) {
    //     std::cout << "[ ";
    //     for (uint32_t x = 0; x < width_; x++) {
    //         std::cout << std::setw(4) << *(pDepthCalc_ + (y * width_) + x) << ' ';
    //     }
    //     std::cout << "]\n";
    // }

    // exit(0);

    /** sets:
     *      height_, width_, size_, pImgLeftRgb_, pImgRightRgb_
     */
    loadImgRgb(pathImgLeft, pathImgRight);

    // TODO DO THIS SOMEWHERE ELSE< CHECK BLOCK SIZE TOO
    if (width_ <= maxDisp_) {
        throw std::runtime_error("Parameter 'maxDisp' must be smaller than image width.");
    }

    /** sets:
     *      pImgLeftGray_, pImgRightGray_
     */
    pImgLeftGray_ = convertTo<PixelGray>(pImgLeftRgb_);
    pImgRightGray_ = convertTo<PixelGray>(pImgRightRgb_);

    /** sets:
     *      pDepthTrue_
     */
    pDepthTrue_ = loadPfm(pathDepthTrue);

    /** sets:
     *      pImgDepthTrue_
     */
    pImgDepthTrue_ = convertTo<PixelRgb>(pDepthTrue_);

    /** sets:
     *      pCost_
     */
    pCost_ = costCalc(pImgLeftGray_, pImgRightGray_);

    /** sets:
     *      pAggr_
     */
    pAggr_ = costAggr();

    //
    pDepthCalc_ = costMin();
    pImgDepthCalc_ = convertTo<PixelRgb>(pDepthCalc_);

    //
    pDepthDiff_ = new float[size_];
    for (uint32_t y = 0; y < height_; y++) {
        for (uint32_t x = 0; x < width_; x++) {
            *(pDepthDiff_ + (y * width_) + x) = 128 + *(pDepthCalc_ + (y * width_) + x) - *(pDepthTrue_ + (y * width_) + x);
        }
    }

    //
    pImgDepthDiff_ = convertTo<PixelRgb>(pDepthDiff_);
}

void Model::loadImgRgb(const std::string &pathLeft, const std::string &pathRight) {
    const auto pngToRgb = [this](const png::image<png::rgb_pixel> &img, PixelRgb *pDst) {
        const auto cast = [](const png::rgb_pixel &pixel) -> PixelRgb { return {pixel.red, pixel.green, pixel.blue}; };
        const png::rgb_pixel *pSrc;
        for (uint32_t y = 0; y < height_; y++) {
            pSrc = img.get_row(y).data();
            for (uint32_t x = 0; x < width_; x++) {
                *(pDst++) = cast(*(pSrc++));
            }
        }
    };

    png::image<png::rgb_pixel> imgLeft(pathLeft), imgRight(pathRight);

    if (imgLeft.get_height() != imgRight.get_height()) {
        throw std::runtime_error("Inconsistent height of left and right images."s);
    }
    if (imgLeft.get_width() != imgRight.get_width()) {
        throw std::runtime_error("Inconsistent width of left and right images."s);
    }

    width_ = imgLeft.get_width();
    height_ = imgLeft.get_height();
    size_ = height_ * width_;

    if (0 == height_) {
        throw std::runtime_error("Invalid image height."s);
    }
    if (0 == width_) {
        throw std::runtime_error("Invalid image width."s);
    }

    pImgLeftRgb_ = new PixelRgb[size_];
    pImgRightRgb_ = new PixelRgb[size_];

    pngToRgb(imgLeft, pImgLeftRgb_);
    pngToRgb(imgRight, pImgRightRgb_);
}

template <typename Dst, typename Src>
Dst *Model::convertTo(const Src *pSrc) {
    Dst *pResult = new Dst[size_], *pDst = pResult;

    for (uint32_t i = 0; i < size_; i++) {
        *(pDst++) = *(pSrc++);
    }

    return pResult;
}

float *Model::loadPfm(const std::string &path) {
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if (!ifs) {
        throw std::runtime_error("Could not open PFM file ('"s + path + "')."s);
    }

    std::string type;
    uint32_t height, width;
    float byteOrder;
    uint8_t cr;

    ifs >> type >> width >> height >> byteOrder;
    ifs.read(reinterpret_cast<char *>(&cr), 1);
    if ((!ifs) || (sizeof(cr) != ifs.gcount())) {
        throw std::runtime_error("Could not read PFM header."s);
    }

    if ("Pf"s != type) {
        throw std::runtime_error("Unsupported PFM file type ('"s + type + "')."s);
    }
    if (height != height_) {
        throw std::runtime_error("Invalid PFM file height ('"s + std::to_string(height) + "')."s);
    }
    if (width != width_) {
        throw std::runtime_error("Invalid PFM file width ('"s + std::to_string(width) + "')"s);
    }

    float *pData = new float[size_];
    {
        uint32_t rowSize = width_ * sizeof(float), y, n;
        for (y = 0; ifs && (y < height_); y++) {
            for (n = 0; ifs && (n < rowSize); n += ifs.gcount()) {
                ifs.read(reinterpret_cast<char *>(pData + ((height_ - 1 - y) * width_)) + n, rowSize - n);
            }
        }
        if (y < height) {
            throw std::runtime_error("Could not read PFM payload."s);
        }
    }

    if (0 < byteOrder) {
        for (uint32_t i = 0; i < size_; i++) {
            uint8_t *pData = reinterpret_cast<uint8_t *>(pData + i);

            pData[0] = std::exchange(pData[3], pData[0]);
            pData[1] = std::exchange(pData[2], pData[1]);
        }
    }

    // Display loaded PFM metadata
    {
        float maxDepth = 0;
        for (uint32_t i = 0; i < size_; i++) {
            float depth = *(pData + i);
            if (std::numeric_limits<float>::infinity() != depth) {
                maxDepth = std::max(maxDepth, depth);
            }
        }
        std::cout << "PFM \"" << path << "\", byteorder=" << byteOrder << ", maxDepth=" << maxDepth << std::endl;
    }

    return pData;
}

template <typename Pixel>
uint32_t *Model::costCalc(const Pixel *pLeft, const Pixel *pRight) {
    /**
     *  HEIGHT * WIDTH * N_DISP
     */
    const uint32_t step = width_ * nDisp_;
    uint32_t *const pCost = new uint32_t[height_ * step];

    for (uint32_t y = 0; y < height_; y++) {
        uint32_t x;
        for (x = 0; x < maxDisp_; x++) {
            for (uint32_t d = 0; d < nDisp_; d++) {
                *(pCost + (y * step) + (x * nDisp_) + d) = 0;
            }
        }
        for (; x < width_; x++) {
            for (uint32_t d = 0; d < nDisp_; d++) {
                *(pCost + (y * step) + (x * nDisp_) + d) = std::abs(*(pLeft + (y * width_) + x) - *(pRight + (y * width_) + x - d));
            }
        }
    }

    return pCost;
}

uint32_t *Model::costAggr() {
    /**
     *  HEIGHT * WIDTH * N_DISP
     */
    const uint32_t step = width_ * nDisp_;

    uint32_t *const pAggr = new uint32_t[height_ * step];

    for (uint32_t y = (blockSize_ / 2); y < (height_ - (blockSize_ / 2)); y++) {
        std::cout << (100.f * y / (height_ - blockSize_ + 1)) << "%" << std::endl;
        for (uint32_t x = ((nDisp_ - 1) + (blockSize_ / 2)); x < (width_ - (blockSize_ / 2)); x++) {
            for (uint32_t d = 0; d < nDisp_; d++) {
                uint32_t cost = 0;

                for (uint32_t i = 0; i < blockSize_; i++) {
                    for (uint32_t j = 0; j < blockSize_; j++) {
                        cost += *(pCost_ + ((y - (blockSize_ / 2) + i) * step) + ((x - (blockSize_ / 2) + j) * nDisp_) + d);
                    }
                }

                *(pAggr + (y * step) + (x * nDisp_) + d) = cost;
            }
        }
    }
    return pAggr;
}

uint32_t *Model::costForPixel(uint32_t x, uint32_t y) { return pAggr_ + (y * width_ * nDisp_) + (x * nDisp_); }

float *Model::costMin() {
    float *const pMin = new float[height_ * width_];

    for (uint32_t y = 0; y < height_; y++) {
        for (uint32_t x = 0; x < width_; x++) {
            const uint32_t *pCost = costForPixel(x, y);
            *(pMin + (y * width_) + x) = std::min_element(pCost, pCost + nDisp_) - pCost;
        }
    }

    return pMin;
}
