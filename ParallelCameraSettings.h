#pragma once

#include "pch.h"

class ParallelCameraSettings {
public:
    ParallelCameraSettings(
        const double* position, const double* focalPoint, const double* viewUp,
        double parallelScale, const double* clippingRange, double azimuth, double elevation, double roll,
        double pitch, double yaw);
    ~ParallelCameraSettings();

    ParallelCameraSettings(const ParallelCameraSettings&);
    ParallelCameraSettings& operator=(const ParallelCameraSettings&);

    ParallelCameraSettings(ParallelCameraSettings&&) noexcept;
    ParallelCameraSettings& operator=(ParallelCameraSettings&&) noexcept;

    void ApplyToCamera(const vtkSmartPointer<vtkCamera>& camera) const noexcept;

    ParallelCameraSettings& AroundFocal(double azimuth, double elevation);
    ParallelCameraSettings& Rotate(double yaw, double pitch, double roll);

    static ParallelCameraSettings FromCamera(const vtkSmartPointer<vtkCamera>& camera);

private:
    double* _position;
    double* _focalPoint;
    double* _viewUp;
    double* _clippingRange;

    double _parallelScale;
    double _azimuth;
    double _elevation;
    double _roll;
    double _yaw;
    double _pitch;
};

