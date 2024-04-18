#include "pch.h"
#include "OpenGLInfoWidgetRenderer.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>


void OpenGLInfoWidgetRenderer::Init()
{
    _init = true;
}

vtkSmartPointer<vtkTexture> OpenGLInfoWidgetRenderer::RenderTableFromText(TableHeadersCollection xLabels,
                                                                          TableHeadersCollection yLabels,
                                                                          TableContentsCollection contents)
{
    return vtkSmartPointer<vtkTexture>();
}

vtkSmartPointer<vtkActor> OpenGLInfoWidgetRenderer::VtkActorFromGLTexture(GLuint oglTexture)
{
    if(!_init)
        Init();

    return {};
}

GLuint OpenGLInfoWidgetRenderer::CreateGlTextTexture(const std::string& text, float fontSize, int* outWidth, int* outHeight, const char* fontPath)
{
    std::ifstream fontFile(fontPath, std::ios::binary);

    if (!fontFile.is_open())
    {
        throw std::runtime_error("Unable to open font file"); // TODO: NO EXCEPTIONS
    }

    std::vector<uchar> buffer(std::istreambuf_iterator(fontFile), {});

    stbtt_fontinfo font;

    if (!stbtt_InitFont(&font, buffer.data(), 0))
    {
        throw std::runtime_error("STB True Type unable to load font");
    }

    float scale = stbtt_ScaleForPixelHeight(&font, fontSize);

    int totalWidth {};
    int maxHeight {};

    std::vector<int> widths(text.size());
    std::vector<int> heights(text.size());
    std::vector<int> xoffs(text.size());
    std::vector<int> yoffs(text.size());
    std::vector<uchar_ptr> bitmaps(text.size());

    for(size_t i = 0; i < text.size(); ++i) 
    {
        bitmaps[i] = stbtt_GetCodepointBitmap(&font, 0, scale, text[i], &widths[i], &heights[i], &xoffs[i], &yoffs[i]);
        totalWidth += widths[i];
        maxHeight = std::max(maxHeight, heights[i]);
    }

    unsigned char* finalBitmap = new unsigned char[totalWidth * maxHeight];
    memset(finalBitmap, 0, totalWidth * maxHeight);

    int xOffset = 0;
    for(size_t i = 0; i < text.size(); ++i) 
    {
        for(int y = 0; y < heights[i]; ++y) 
        {
            for(int x = 0; x < widths[i]; ++x) 
            {
                finalBitmap[(y * totalWidth) + xOffset + x] = bitmaps[i][(y * widths[i]) + x];
            }
        }
        xOffset += widths[i];
        stbtt_FreeBitmap(bitmaps[i], NULL);
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, totalWidth, maxHeight, 0, GL_RED, GL_UNSIGNED_BYTE, finalBitmap);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(outWidth != nullptr) *outWidth = totalWidth;
    if(outHeight != nullptr) *outHeight = maxHeight;

    delete[] finalBitmap;
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}
