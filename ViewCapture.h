#pragma once

#include "pch.h"

#include "ParallelCameraSettings.h"
#include "Size.hpp"


class ViewCapture
{
public:
    ViewCapture() = default;

    //
    void TakeScreenshot(const vtkActorCollection*, const Size&, const std::string&) const;

    void SetCameraConfiguration(const ParallelCameraSettings& cfg) const;

private:
    // 
    ParallelCameraSettings* _settings = nullptr;
};

