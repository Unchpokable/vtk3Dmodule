#include "pch.h"
#include "ViewCapture.h"
#include "vtkUtils.hpp"

void ViewCapture::TakeScreenshot(const vtkActorCollection* sceneObjects, const Size& size, const std::string& outputPath) const
{
    const vtkNew<vtkRenderer> renderer {};
    const vtkNew<vtkRenderWindow> renderWindow {};

    AddActorsToRenderer(renderer, const_cast<vtkActorCollection*>(sceneObjects));

    renderWindow->AddRenderer(renderer);
    renderWindow->SetOffScreenRendering(true);
    renderWindow->SetSize(size.Width(), size.Height());

    const auto camera = renderer->GetActiveCamera();

    if (_settings)
        _settings->ApplyToCamera(camera);

    renderWindow->Render();

    const vtkNew<vtkWindowToImageFilter> wnd2Img;

    wnd2Img->SetInput(renderWindow);
    wnd2Img->Update();

    const vtkNew<vtkPNGWriter> writer;
    writer->SetFileName(outputPath.c_str());

    writer->SetInputConnection(wnd2Img->GetOutputPort());
    writer->Write();
}


void ViewCapture::SetCameraConfiguration(const ParallelCameraSettings& cfg) const
{
    *_settings = cfg;
}
