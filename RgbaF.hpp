#pragma once

#include "pch.h"

#include <array>

class RgbaF {
public:
    RgbaF()
        : _r(0.0f), _g(0.0f), _b(0.0f), _a(1.0f) {
    }

    RgbaF(float r, float g, float b, float a = 1.0f) {
        SetR(r);
        SetG(g);
        SetB(b);
        SetA(a);
    }

    RgbaF(const RgbaF& other) = default;
    RgbaF(RgbaF&& other) noexcept = default;
    RgbaF& operator=(const RgbaF& other) = default;
    RgbaF& operator=(RgbaF&& other) noexcept = default;
    ~RgbaF() = default;

    float R() const
    {
        return _r;
    }

    void SetR(float r)
    {
        _r = Clamp(r);
    }

    float G() const
    {
        return _g;
    }
    void SetG(float g)
    {
        _g = Clamp(g);
    }

    float B() const
    {
        return _b;
    }
    void SetB(float b)
    {
        _b = Clamp(b);
    }

    float A() const
    {
        return _a;
    }
    void SetA(float a)
    {
        _a = Clamp(a);
    }

    std::array<double, 3> ToRGBDoubleArray() const
    {
        return { static_cast<double>(_r), static_cast<double>(_g), static_cast<double>(_b) };
    }

    Eigen::Vector3d ToEigenVector3d() const
    {
        return { _r, _g, _b };
    }

    static RgbaF FromQColor(const QColor& color) {
        return { color.redF(), color.greenF(), color.blueF(), color.alphaF() };
    }

private:
    float _r;
    float _g;
    float _b;
    float _a;

    // just for avoid passing min and max and set it by default
    static constexpr float Clamp(float value, float min = 0.0f, float max = 1.0f) {
        return std::clamp(value, min, max);
    }
};