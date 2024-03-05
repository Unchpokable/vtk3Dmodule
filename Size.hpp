#pragma once

#include "pch.h"

class Size
{
public:
    Size() : _width(0), _height(0)
    {
    }

    Size(int width, int height) : _width(width), _height(height)
    {
    }

    Size(const Size& other)  = default;

    Size(Size&& other) noexcept : _width(other._width), _height(other._height)
    {
        other._width = 0;
        other._height = 0;
    }

    Size& operator=(const Size& other)
    {
        if(this != &other) 
        {
            _width = other._width;
            _height = other._height;
        }
        return *this;
    }

    Size& operator=(Size&& other) noexcept
    {
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

    int& Width()
    {
        return _width;
    }

    int& Height()
    {
        return _height;
    }

private:
    int _width, _height;
};