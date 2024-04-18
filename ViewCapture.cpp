#include "pch.h"
#include "ViewCapture.h"
#include "vtkUtils.hpp"


ViewCapture::ViewCapture(const vtkSmartPointer<vtkRenderer>& target)
{
    _target = target;
}

std::string ViewCapture::TakeScreenshot(const vtkActorCollection* sceneObjects, const Size& size, const std::string& outputPath, PreShotAction prepareAction, bool drawFrame) const
{
    const vtkNew<vtkRenderer> renderer {};
    const vtkNew<vtkRenderWindow> renderWindow {};

    AddActorsToRenderer(renderer, const_cast<vtkActorCollection*>(sceneObjects));

    if (drawFrame)
        SetActorsToWireframeDisplay(renderer);


    if(prepareAction)
        prepareAction(renderer);
    else
        renderer->SetBackground(1, 1, 1);

    renderWindow->AddRenderer(renderer);
    renderWindow->SetOffScreenRendering(true);
    renderWindow->SetSize(size.Width(), size.Height());

    const auto camera = renderer->GetActiveCamera();

    camera->ParallelProjectionOn();

    if (_settingsProxy)
        camera->ShallowCopy(_settingsProxy);

    if (_rawSettings)
        _rawSettings->ApplyToCamera(camera);

    renderer->ResetCamera();

    renderWindow->Render();

    const vtkNew<vtkWindowToImageFilter> wnd2Img;

    wnd2Img->SetInput(renderWindow);
    wnd2Img->Update();

    const vtkNew<vtkPNGWriter> writer;
    writer->SetFileName(outputPath.c_str());

    writer->SetInputConnection(wnd2Img->GetOutputPort());
    writer->Write();

    SetActorsToSurfaceDisplay(renderer);

    return absolute(std::filesystem::path(outputPath)).string();
}

std::string ViewCapture::TakeScreenshot(const Size& size, const std::string& path, const PreShotAction& prepareAction, bool drawFrame) const
{

    return TakeScreenshot(const_cast<vtkRenderer*>(_target)->GetActors(), size, path, prepareAction, drawFrame);
}


void ViewCapture::SetBaseCamera(const vtkSmartPointer<vtkCamera>& sourceCam)
{
    if (!_settingsProxy || !_settingsProxy.GetPointer())
        _settingsProxy = vtkSmartPointer<vtkCamera>::New();

}

void ViewCapture::SetSpecialCameraConfigs(ParallelCameraSettings& conf)
{
    if (_rawSettings)
        *_rawSettings = conf;

    else _rawSettings = new ParallelCameraSettings(conf);
}

void ViewCapture::AddSpecialActors(const std::vector<vtkSmartPointer<vtkProp>>& actors) {
    _additionalActors = actors;
}
