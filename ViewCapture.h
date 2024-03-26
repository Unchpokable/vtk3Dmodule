#pragma once

#include "pch.h"

#include "ParallelCameraSettings.h"
#include "Size.hpp"


class ViewCapture
{
public:

    using PreShotAction = std::function<void(const vtkRenderer*)>;

    ViewCapture() = default;

    ViewCapture(const vtkSmartPointer<vtkRenderer>& target);

    //
    std::string TakeScreenshot(const vtkActorCollection*, const Size&, const std::string&, bool drawFrame = false) const;

    std::string TakeScreenshot(const Size&, const std::string&, PreShotAction = nullptr, bool drawFrame = false) const;


    void SetBaseCamera(const vtkSmartPointer<vtkCamera>&);
    void SetSpecialCameraConfigs(ParallelCameraSettings&);

private:
    const vtkRenderer* _target;
    vtkSmartPointer<vtkCamera> _settingsProxy = nullptr;

    ParallelCameraSettings* _rawSettings = nullptr;
};

