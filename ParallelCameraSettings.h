#pragma once

#include "pch.h"

class ParallelCameraSettings {
public:
    ParallelCameraSettings(
        const double* position, const double* focalPoint, const double* viewUp,
        double parallelScale, const double* clippingRange);
    ~ParallelCameraSettings();

    ParallelCameraSettings(const ParallelCameraSettings&);
    ParallelCameraSettings& operator=(const ParallelCameraSettings&);

    ParallelCameraSettings(ParallelCameraSettings&&) noexcept;
    ParallelCameraSettings& operator=(ParallelCameraSettings&&) noexcept;

    void ApplyToCamera(const vtkSmartPointer<vtkCamera>& camera) const noexcept;
    static ParallelCameraSettings FromCamera(const vtkSmartPointer<vtkCamera>& camera);

private:
    double* _position;
    double* _focalPoint;
    double* _viewUp;
    double _parallelScale;
    double* _clippingRange;
};

