#pragma once

#include "pch.h"

#include "ParallelCameraSettings.h"
#include "Size.hpp"

struct CameraAngle {
    double Azimuth;
    double Elevation;
};

class ViewCapture {
public:
    using AngleSeries = std::vector<CameraAngle>;
    using PreShotAction = std::function<void(vtkRenderer*)>;

    ViewCapture(const vtkSmartPointer<vtkRenderer>& target);
    std::string TakeScreenshot(const Size&, const std::string&, const PreShotAction & = nullptr, bool drawFrame = false) const;

    vtkSmartPointer<vtkUnsignedCharArray> TakeScreenshotToMemory(const Size&, const PreShotAction & = nullptr, bool drawFrame = false) const;
    QPixmap TakeScreenshotToQPixMap(const Size&, const PreShotAction & = nullptr, bool drawFrame = false) const;
    std::string TakeScreenshotToBase64(const Size&, const PreShotAction & = nullptr, bool drawFrame = false) const;

    std::vector<vtkSmartPointer<vtkImageData>> MakeScreenshotSeries(const AngleSeries& angles, const Size& size, const PreShotAction& preShot = nullptr, bool drawFrame = false) const;

    void SetBaseCamera(const vtkSmartPointer<vtkCamera>&);
    void SetSpecialCameraConfigs(ParallelCameraSettings&);
    void AddSpecialActors(const std::vector<vtkSmartPointer<vtkProp>>&);

    void EnableWinApiTricks() const noexcept;
    void DisableWinApiTricks() const noexcept;

private:
    const vtkRenderer* _target;
    vtkSmartPointer<vtkCamera> _settingsProxy = nullptr;

    std::vector<vtkSmartPointer<vtkProp>> _additionalActors;
    ParallelCameraSettings* _rawSettings = nullptr;

    vtkSmartPointer<vtkRenderWindow> _renderWindow;
    vtkSmartPointer<vtkRenderer> _renderer;
    vtkSmartPointer<vtkRenderWindowInteractor> _interactor;

    mutable bool _windowsApiTricksEnabled = false;

    vtkImageData* TakeScreenshot(const vtkActorCollection *sceneObjects, const Size &size, const PreShotAction & = nullptr, bool drawFrame = false) const;
    static QPixmap vtkImageToQPixmap(vtkImageData *imageData);
};

