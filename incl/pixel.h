#pragma once

#include <cstdint>
#include <limits>

struct PixelRgb {
    uint8_t r, g, b;

    PixelRgb &operator=(const float &value) {
        r = g = b = ((std::numeric_limits<float>::infinity() != value) ? (static_cast<uint8_t>(std::min(std::abs(value), 255.0f))) : (static_cast<uint8_t>(0u)));
        return *this;
    }
};

struct PixelGray {
    uint8_t v[3];

    PixelGray &operator=(const PixelRgb &pixelRgb) {
        v[2] = v[1] = v[0] = (0.21f * pixelRgb.r) + (0.72f * pixelRgb.g) + (0.07f * pixelRgb.b);
        return *this;
    }

    operator PixelRgb() { return {v[0], v[1], v[2]}; }

    PixelGray &operator=(const float &value) {
        v[2] = v[1] = v[0] = ((std::numeric_limits<float>::infinity() != value) ? (static_cast<uint8_t>(std::min(std::abs(value), 255.0f))) : (static_cast<uint8_t>(0u)));
        return *this;
    }

    int16_t operator-(const PixelGray &other) const { return static_cast<int16_t>(v[0]) - static_cast<int16_t>(other.v[0]); }
};
