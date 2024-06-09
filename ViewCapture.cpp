#include "pch.h"
#include "ViewCapture.h"
#include "vtkUtils.hpp"

#include <QPixmap>
#include <QImage>
#include <vtkUnsignedCharArray.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkPointData.h>
#include <vtkImageFlip.h>
#include <vtkImageCast.h>
#include <vtkImageMapToColors.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageReslice.h>
#include <vtkBase64OutputStream.h>

#ifdef _WIN64
#include "Windows.h"
#else
inline BOOL ShowWindow(HWND hwnd, int nCmdShow) {
    throw std::exception("Unsupported for current platform");
}
#endif

ViewCapture::ViewCapture(const vtkSmartPointer<vtkRenderer>& target) {
    _target = target;
    _renderWindow = vtkSmartPointer<vtkRenderWindow>::New();

    _renderer = vtkSmartPointer<vtkRenderer>::New();
    _interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();

    _renderWindow->AddRenderer(_renderer);
    _renderWindow->SetInteractor(_interactor);

#ifdef _WIN64
    const auto hwnd = static_cast<HWND>(_renderWindow->GetGenericWindowId());
    ShowWindow(hwnd, SW_HIDE);
#else
    _renderWindow->SetOffScreenRendering(true);
#endif
}

std::string ViewCapture::TakeScreenshot(const Size& size, const std::string& path, const PreShotAction& prepareAction, bool drawFrame) const {
    const auto imageData = TakeScreenshot(const_cast<vtkRenderer*>(_target)->GetActors(),
        size, prepareAction, drawFrame);

    const vtkNew<vtkPNGWriter> writer;
    writer->SetInputData(imageData);
    writer->Write();
    return absolute(std::filesystem::path(path)).string();
}

vtkSmartPointer<vtkUnsignedCharArray> ViewCapture::TakeScreenshotToMemory(const Size& size, const PreShotAction& prepareAction, bool drawFrame) const {
    const auto imageData = TakeScreenshot(const_cast<vtkRenderer*>(_target)->GetActors(),
        size, prepareAction, drawFrame);

    const vtkNew<vtkPNGWriter> writer;
    writer->SetInputData(imageData);
    writer->WriteToMemoryOn();
    writer->Write();
    return writer->GetResult();
}

QPixmap ViewCapture::TakeScreenshotToQPixMap(const Size &size, const PreShotAction &preShot, bool drawFrame) const {
    auto imageData = TakeScreenshot(const_cast<vtkRenderer*>(_target)->GetActors(), size, preShot, drawFrame);
    return vtkImageToQPixmap(imageData);
}

std::string ViewCapture::TakeScreenshotToBase64(const Size& size, const PreShotAction& prepareAction, bool drawFrame) const {
    const auto imageData = TakeScreenshot(const_cast<vtkRenderer*>(_target)->GetActors(),
        size, prepareAction, drawFrame);

    const vtkNew<vtkPNGWriter> writer;

    writer->SetInputData(imageData);
    writer->WriteToMemoryOn();
    writer->Write();

    const auto result = writer->GetResult();

    std::ostringstream b64Stream;
    const vtkNew<vtkBase64OutputStream> b64Output;

    b64Output->SetStream(&b64Stream);
    b64Output->StartWriting();
    b64Output->Write(result->GetPointer(0), result->GetNumberOfTuples());
    b64Output->EndWriting();

    return b64Stream.str();
}

std::vector<vtkSmartPointer<vtkImageData>> ViewCapture::MakeScreenshotSeries(const AngleSeries& angles, const Size & size, const PreShotAction & preShot, bool drawFrame) const {
    std::vector<vtkSmartPointer<vtkImageData>> out;

    for(const auto& angle : angles) {
        out.push_back(TakeScreenshot(const_cast<vtkRenderer*>(_target)->GetActors(), size, preShot, drawFrame));
    }

    return out;
}

QPixmap ViewCapture::vtkImageToQPixmap(vtkImageData * imageData) {
    if(!imageData) {
        return {};
    }

    auto castFilter = vtkSmartPointer<vtkImageCast>::New();
    castFilter->SetInputData(imageData);
    castFilter->SetOutputScalarTypeToUnsignedChar();
    castFilter->Update();

    vtkImageData* castedImage = castFilter->GetOutput();

    int width = castedImage->GetDimensions()[0];
    int height = castedImage->GetDimensions()[1];
    int numComponents = castedImage->GetNumberOfScalarComponents();

    vtkUnsignedCharArray* dataArray = vtkUnsignedCharArray::SafeDownCast(castedImage->GetPointData()->GetScalars());
    if(!dataArray) {
        return QPixmap();
    }

    unsigned char* rawData = dataArray->GetPointer(0);

    QImage::Format format = (numComponents == 4) ? QImage::Format_RGBA8888 : QImage::Format_RGB888;
    QImage image(rawData, width, height, width * numComponents, format);

    auto pixmap = QPixmap::fromImage(image);

    return pixmap;
}

vtkImageData* ViewCapture::TakeScreenshot(const vtkActorCollection* sceneObjects,
    const Size& size, const PreShotAction& prepareAction, bool drawFrame) const {
    AddActorsToRenderer(_renderer, const_cast<vtkActorCollection*>(sceneObjects));

    if(!_additionalActors.empty()) {
        AddActorsToRenderer(_renderer, _additionalActors);
    }

    if(drawFrame)
        SetActorsToWireframeDisplay(_renderer);

    if(prepareAction)
        prepareAction(_renderer);
    else
        _renderer->SetBackground(1, 1, 1);

    _renderWindow->SetSize(size.Width(), size.Height());

    const auto camera = _renderer->GetActiveCamera();

    camera->ParallelProjectionOn();

    if(_settingsProxy)
        camera->ShallowCopy(_settingsProxy);

    if(_rawSettings)
        _rawSettings->ApplyToCamera(camera);

    _renderer->ResetCamera();

    _renderWindow->Render();

    const vtkNew<vtkWindowToImageFilter> wnd2Img;

    wnd2Img->SetInput(_renderWindow);
    wnd2Img->Update();

    return wnd2Img->GetOutput();
}

void ViewCapture::SetBaseCamera(const vtkSmartPointer<vtkCamera>& sourceCam) {
    if(!_settingsProxy || !_settingsProxy.GetPointer())
        _settingsProxy = vtkSmartPointer<vtkCamera>::New();

}

void ViewCapture::SetSpecialCameraConfigs(ParallelCameraSettings& conf) {
    if(_rawSettings)
        *_rawSettings = conf;

    else _rawSettings = new ParallelCameraSettings(conf);
}

void ViewCapture::AddSpecialActors(const std::vector<vtkSmartPointer<vtkProp>>& actors) {
    _additionalActors = actors;
}

void ViewCapture::EnableWinApiTricks() const noexcept {
#ifdef _WIN64
    _windowsApiTricksEnabled = true;
#else
    throw std::exception("Unable to activate WinAPI tricks - unsupported platform")
#endif
}

void ViewCapture::DisableWinApiTricks() const noexcept {
    _windowsApiTricksEnabled = false;
}
