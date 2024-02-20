#pragma once

#include "pch.h"

#include "LineData.hpp"
#include "SphereData.hpp"
#include "vtkUtils.hpp"

enum class GeometryPrimitive
{
    Sphere,
    Line
};

enum class DetailsLevel
{
    LaraCroftFrom90s = 2,
    UltraLow = 3,
    Low = 4,
    Medium = 5,
    High = 6,
    ExtraHigh = 7,
    KillMyGraphicsCard = 8,
    OutOfMemory = 10,
};

inline vtkActorPointer CreateRandomSphere(DetailsLevel resolution = DetailsLevel::Medium)
{
    auto sphereSource = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetRadius(std::rand() % 10 + 1);
    sphereSource->SetCenter(std::rand() % 100 - 50, std::rand() % 100 - 50, std::rand() % 100 - 50);

    const auto res = std::pow(2, static_cast<int>(resolution));

    sphereSource->SetPhiResolution(res);
    sphereSource->SetThetaResolution(res);

    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(sphereSource->GetOutputPort());
    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(std::rand() / (double)RAND_MAX, std::rand() / (double)RAND_MAX, std::rand() / (double)RAND_MAX);
    return actor;
}

inline vtkActorPointer CreateRandomLine()
{
    auto lineSource = vtkSmartPointer<vtkLineSource>::New();
    lineSource->SetPoint1(std::rand() % 20 - 10, std::rand() % 20 - 10, std::rand() % 20 - 10);
    lineSource->SetPoint2(std::rand() % 20 - 10, std::rand() % 20 - 10, std::rand() % 20 - 10);

    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(lineSource->GetOutputPort());

    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(std::rand() / (double)RAND_MAX, std::rand() / (double)RAND_MAX, std::rand() / (double)RAND_MAX);
    return actor;
}

inline vtkActorPointer CreateSphere(double radius, const Eigen::Vector3d& center, const Eigen::Vector3d& color, DetailsLevel resoulution = DetailsLevel::Medium)
{
    const auto sphereSource = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetRadius(radius);
    sphereSource->SetCenter(center[0], center[1], center[2]);

    const auto res = std::pow(2, static_cast<int>(resoulution));

    sphereSource->SetPhiResolution(res);
    sphereSource->SetThetaResolution(res);

    const auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(sphereSource->GetOutputPort());
    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(color[0], color[1], color[2]);
    return actor;
}

inline vtkActorPointer CreateSphere(const SphereData& data)
{
    return CreateSphere(data.Radius, data.Position, data.Color.ToEigenVector3d());
}

inline vtkActorPointer CreateLine(const Eigen::Vector3d& begin, const Eigen::Vector3d& end, const Eigen::Vector3d& color = Eigen::Vector3d(1, 1, 1))
{
    const auto lineSource = vtkSmartPointer<vtkLineSource>::New();

    lineSource->SetPoint1(begin[0], begin[1], begin[2]);
    lineSource->SetPoint2(end[0], end[1], end[2]);

    const auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(lineSource->GetOutputPort());

    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(color[0], color[1], color[2]);

    return actor;
}

inline vtkActorPointer CreateLine(const LineData& data)
{
    return CreateLine(data.Begin, data.End, data.Color.ToEigenVector3d());
}

inline std::vector<vtkSmartPointer<vtkActor>> CreateRandomObjectsOfType(GeometryPrimitive type, size_t count)
{
    std::vector<vtkSmartPointer<vtkActor>> actors {};
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    switch(type)
    {
        case GeometryPrimitive::Sphere:
        {
            for(size_t i = 0; i < count; ++i)
                actors.push_back(CreateRandomSphere());
            break;
        }
        case GeometryPrimitive::Line:
        {
            for(size_t i = 0; i < count; ++i)
                actors.push_back(CreateRandomLine());
            break;
        }
    }

    return actors;
}