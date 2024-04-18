#pragma once

#include "pch.h"

using uchar = unsigned char;
using uchar_ptr = unsigned char*;

class OpenGLInfoWidgetRenderer
{
public:
    using TableHeadersCollection = const std::vector<std::string>&;
    using TableContentsCollection = const std::vector<std::vector<std::string>>&;

    struct CellFormat
    {
        enum class VerticalAlignment
        {
            Top, Center, Bottom
        };

        enum class HorizontalAlignment
        {
            Left, Center, Right
        };

        int Padding;

        VerticalAlignment VerticalAlignment;
        HorizontalAlignment HorizontalAlignment;

        static CellFormat Default()
        {
            CellFormat result;
            result.Padding = 5;
            result.VerticalAlignment = VerticalAlignment::Center;
            result.HorizontalAlignment = HorizontalAlignment::Center;

            return result;
        }
    };

    struct Point
    {
        int X, Y;

        explicit Point(int x = 0, int y = 0) : X(x), Y(y) {
        }
    };

    static void Init();
    static vtkSmartPointer<vtkTexture> RenderTableFromText(TableHeadersCollection xLabels, 
                                                           TableHeadersCollection yLabels, 
                                                           TableContentsCollection contents);

private:
    static vtkSmartPointer<vtkActor> VtkActorFromGLTexture(GLuint oglTexture);
    static GLuint CreateGlTextTexture(const std::string& text, float fontSize, int* outWidth, int* outHeight, const char* fontPath = "c:\\windows\\fonts\\tahoma.ttf");

    static inline bool _init = false;
};

