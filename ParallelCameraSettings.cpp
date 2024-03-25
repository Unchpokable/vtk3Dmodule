#include "pch.h"
#include "ParallelCameraSettings.h"

ParallelCameraSettings::ParallelCameraSettings(
    const double* position, const double* focalPoint, const double* viewUp,
    double parallelScale, const double* clippingRange)
    : _parallelScale(parallelScale)
{
    _position = new double[3];
    std::copy_n(position, 3, _position);

    _focalPoint = new double[3];
    std::copy_n(focalPoint, 3, _focalPoint);

    _viewUp = new double[3];
    std::copy_n(viewUp, 3, _viewUp);

    _clippingRange = new double[2];
    std::copy_n(clippingRange, 2, _clippingRange);
}

ParallelCameraSettings::ParallelCameraSettings(const ParallelCameraSettings& other)
    : _parallelScale(other._parallelScale)
{
    _position = new double[3];
    std::copy_n(other._position, 3, _position);

    _focalPoint = new double[3];
    std::copy_n(other._focalPoint, 3, _focalPoint);

    _viewUp = new double[3];
    std::copy_n(other._viewUp, 3, _viewUp);

    _clippingRange = new double[2];
    std::copy_n(other._clippingRange, 2, _clippingRange);
}

ParallelCameraSettings& ParallelCameraSettings::operator=(const ParallelCameraSettings& other) {
    if(this != &other) {
        delete[] _position;
        delete[] _focalPoint;
        delete[] _viewUp;
        delete[] _clippingRange;

        _parallelScale = other._parallelScale;

        _position = new double[3];
        std::copy_n(other._position, 3, _position);

        _focalPoint = new double[3];
        std::copy_n(other._focalPoint, 3, _focalPoint);

        _viewUp = new double[3];
        std::copy_n(other._viewUp, 3, _viewUp);

        _clippingRange = new double[2];
        std::copy_n(other._clippingRange, 2, _clippingRange);
    }
    return *this;
}

ParallelCameraSettings::~ParallelCameraSettings()
{
    delete[] _position;
    delete[] _focalPoint;
    delete[] _viewUp;
    delete[] _clippingRange;
}

ParallelCameraSettings::ParallelCameraSettings(ParallelCameraSettings&& other) noexcept
    : _position(other._position), _focalPoint(other._focalPoint),
    _viewUp(other._viewUp), _parallelScale(other._parallelScale),
    _clippingRange(other._clippingRange)
{
    other._position = nullptr;
    other._focalPoint = nullptr;
    other._viewUp = nullptr;
    other._clippingRange = nullptr;
}

ParallelCameraSettings& ParallelCameraSettings::operator=(ParallelCameraSettings&& other) noexcept {
    if(this != &other) 
    {
        delete[] _position;
        delete[] _focalPoint;
        delete[] _viewUp;
        delete[] _clippingRange;

        _position = other._position;
        _focalPoint = other._focalPoint;
        _viewUp = other._viewUp;
        _parallelScale = other._parallelScale;
        _clippingRange = other._clippingRange;

        other._position = nullptr;
        other._focalPoint = nullptr;
        other._viewUp = nullptr;
        other._clippingRange = nullptr;
    }
    return *this;
}

void ParallelCameraSettings::ApplyToCamera(const vtkSmartPointer<vtkCamera>& camera) const noexcept
{
    camera->SetPosition(_position);
    camera->SetFocalPoint(_focalPoint);
    camera->SetViewUp(_viewUp);
    camera->SetParallelProjection(1);
    camera->SetParallelScale(_parallelScale);
    camera->SetClippingRange(_clippingRange);
}

ParallelCameraSettings ParallelCameraSettings::FromCamera(const vtkSmartPointer<vtkCamera>& camera)
{
    double position[3];
    double focalPoint[3];
    double viewUp[3];
    double clippingRange[2];
    const double parallelScale = camera->GetParallelScale();

    camera->GetPosition(position);
    camera->GetFocalPoint(focalPoint);
    camera->GetViewUp(viewUp);
    camera->GetClippingRange(clippingRange);

    return { position, focalPoint, viewUp, parallelScale, clippingRange };
}

