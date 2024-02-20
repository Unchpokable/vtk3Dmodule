#pragma once

#include "pch.h"

#include "Property.hpp"

#define ALLOW_EXPERIMENTAL_CODE

class Size {
public:
    Size() : _width(0), _height(0) {
    }

    Size(int width, int height) : _width(width), _height(height) {
    }

    Size(const Size& other)  = default;

    Size(Size&& other) noexcept : _width(other._width), _height(other._height) {
        other._width = 0;
        other._height = 0;
    }

    Size& operator=(const Size& other) {
        if(this != &other) {
            _width = other._width;
            _height = other._height;
        }
        return *this;
    }

    Size& operator=(Size&& other) noexcept {
        if(this != &other) {
            _width = other._width;
            _height = other._height;
            other._width = 0;
            other._height = 0;
        }
        return *this;
    }

    bool operator==(const Size& r) const
    {
        return _height == r._height && _width == r._width;
    }

    bool operator!=(const Size& r) const
    {
        return _height != r._height || _width != r._width;
    }

#ifdef ALLOW_EXPERIMENTAL_CODE
    Property<int> Width()
    {
        return Property<int>(_width, [](int w) { return w >= 0; });
    }

    Property<int> Height()
    {
        return Property<int>(_height, [](int h) { return h >= 0; });
    }
#else
    int Width() const
    {
        return _width;
    }

    int Height() const
    {
        return _height;
    }
#endif

private:
    int _width, _height;
};