#pragma once

#include "pch.h"

#include "LineData.hpp"
#include "SphereData.hpp"
#include "vtkUtils.hpp"
#include "xmltools.hpp"

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
    sphereSource->Update();
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

    lineSource->Update();
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(lineSource->GetOutputPort());

    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(std::rand() / (double)RAND_MAX, std::rand() / (double)RAND_MAX, std::rand() / (double)RAND_MAX);
    return actor;
}

inline vtkActorPointer CreateSphere(double radius, const Eigen::Vector3d& center, const Eigen::Vector3d& color, DetailsLevel resolution = DetailsLevel::Medium)
{
    const auto sphereSource = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetRadius(radius);
    sphereSource->SetCenter(center[0], center[1], center[2]);

    const auto res = std::pow(2, static_cast<int>(resolution));

    sphereSource->SetPhiResolution(res);
    sphereSource->SetThetaResolution(res);
    sphereSource->Update();
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

inline vtkActorPointer CreateCone(double radius, double height, const Eigen::Vector3d& center, const Eigen::Vector3d& color, DetailsLevel resolution = DetailsLevel::Medium)
{
    const auto coneSource = vtkSmartPointer<vtkConeSource>::New();
    coneSource->SetRadius(radius);
    coneSource->SetHeight(height);
    coneSource->SetCenter(center[0], center[1], center[2]);
    const auto res = std::pow(2, static_cast<int>(resolution));
    coneSource->SetResolution(res);
    coneSource->Update();
    const auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(coneSource->GetOutputPort());
    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(color[0], color[1], color[2]);

    return actor;
}

inline vtkActorPointer CreateCylinder(double radius, double height, const Eigen::Vector3d& center, const Eigen::Vector3d& color, DetailsLevel resolution = DetailsLevel::Medium)
{
    const auto cylinderSource = vtkSmartPointer<vtkCylinderSource>::New();
    cylinderSource->SetRadius(radius);
    cylinderSource->SetHeight(height);
    cylinderSource->SetCenter(center[0], center[1], center[2]);

    const auto res = std::pow(2, static_cast<int>(resolution));
    cylinderSource->SetResolution(res);
    cylinderSource->Update();
    const auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(cylinderSource->GetOutputPort());
    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(color[0], color[1], color[2]);

    return actor;
}

inline vtkActorPointer CreateTruncatedCone(double topRadius, double bottomRadius, double height, const Eigen::Vector3d& center, const Eigen::Vector3d& color, DetailsLevel resolution = DetailsLevel::Medium)
{
    // Fuck my life then
    const int numSides = std::pow(2, static_cast<int>(resolution));
    const auto points = vtkSmartPointer<vtkPoints>::New();
    const auto polygons = vtkSmartPointer<vtkCellArray>::New();

    // Generate a circles for top and bottom of our cone
    for(vtkIdType i = 0; i < numSides; ++i) {
        double angle = 2.0 * vtkMath::Pi() * i / numSides;
        // Align our cone along the Y axis as any other VTK generators
        // Bottom 
        points->InsertNextPoint(bottomRadius * cos(angle), -height/2, bottomRadius * sin(angle));
        // top
        points->InsertNextPoint(topRadius * cos(angle), height/2, topRadius * sin(angle));
    }

    // Top circle polygons
    const auto topPolygon = vtkSmartPointer<vtkPolygon>::New();
    topPolygon->GetPointIds()->SetNumberOfIds(numSides);
    for(vtkIdType i = 0; i < numSides; ++i) 
    {
        topPolygon->GetPointIds()->SetId(i, i * 2 + 1);
    }
    polygons->InsertNextCell(topPolygon);

    // Bottom circle polygons
    const auto bottomPolygon = vtkSmartPointer<vtkPolygon>::New();
    bottomPolygon->GetPointIds()->SetNumberOfIds(numSides);
    for(vtkIdType i = 0; i < numSides; ++i) {
        bottomPolygon->GetPointIds()->SetId(i, i * 2);
    }
    polygons->InsertNextCell(bottomPolygon);

    // Wall
    for(vtkIdType i = 0; i < numSides; ++i) {
        auto wallPolygon = vtkSmartPointer<vtkPolygon>::New();
        wallPolygon->GetPointIds()->SetNumberOfIds(4); // Rectangle polygons
        const vtkIdType nextIndex = (i + 1) % numSides;
        wallPolygon->GetPointIds()->SetId(0, i * 2);
        wallPolygon->GetPointIds()->SetId(1, nextIndex * 2);
        wallPolygon->GetPointIds()->SetId(2, nextIndex * 2 + 1);
        wallPolygon->GetPointIds()->SetId(3, i * 2 + 1);

        polygons->InsertNextCell(wallPolygon);
    }

    auto polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints(points);
    polyData->SetPolys(polygons);

    vtkNew<vtkPolyDataNormals> normalsFilter;

    normalsFilter->SetInputData(polyData);
    normalsFilter->ComputeCellNormalsOn();
    normalsFilter->ComputePointNormalsOn();
    normalsFilter->ConsistencyOn();
    normalsFilter->AutoOrientNormalsOn();

    normalsFilter->Update();
    
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(normalsFilter->GetOutput());
    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(color[0], color[1], color[2]);
    actor->SetPosition(center[0], center[1], center[2]);

    return actor;
}

inline vtkActorPointer CreateLine(const Eigen::Vector3d& begin, const Eigen::Vector3d& end, const Eigen::Vector3d& color = Eigen::Vector3d(1, 1, 1))
{
    const auto lineSource = vtkSmartPointer<vtkLineSource>::New();

    lineSource->SetPoint1(begin[0], begin[1], begin[2]);
    lineSource->SetPoint2(end[0], end[1], end[2]);
    lineSource->Update();
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


inline Eigen::Vector3d ConvertHexToEigenVectorColor(const QString& hex)
{
    if(!hex.startsWith("#") || hex.length() != 7) {
        return {};
    }

    bool ok;

    const double r = hex.mid(1, 2).toInt(&ok, 16);
    const double g = hex.mid(3, 2).toInt(&ok, 16);
    const double b = hex.mid(5, 2).toInt(&ok, 16);

    return Eigen::Vector3d(r / 255, g / 255, b / 255);
}

using ProbeHeadGeometry = ProbeHeadExtensionGeometryData;

inline Result<vtkActorPointer> BuildGeometryFromGeometryPrimitive(const ProbeHeadGeometry& geometry) {
    const auto color = ConvertHexToEigenVectorColor(geometry.HexColor());
    switch(geometry.Type()) {
        case GeometryType::Cylinder:
            return CreateCylinder(geometry.Diameter(), geometry.Height(), {0, 0, 0}, color);
            
        case GeometryType::Cone:
            return CreateTruncatedCone(geometry.Diameter1(), geometry.Diameter2(), geometry.Height(), { 0, 0, 0 }, color);
        case GeometryType::Sphere:
            //Looks like a `Diameter` from part catalog is a radius actually POG LOL KEK
            return CreateSphere(geometry.Diameter(), { 0, 0, 0 }, color);
    }
    return { Status::GenericFailure, "Uncovered geometry type enum value" };
}


// Generate a single poly-data actor from list of an geometry container
inline vtkActorPointer GeneratePolyPart(const GeometryDataContainer& part)
{
    //TODO: Too complex code. Split into separate functions
    double runningHeight = 0;

    std::vector<vtkActorPointer> actors;

    for(const auto& g : part) {
        const auto geometry = BuildGeometryFromGeometryPrimitive(g);
        if(geometry.isSuccess()) {
            const auto actor = geometry.value();
            actor->SetPosition(0, runningHeight + g.Height() / 2, 0);
            runningHeight += g.Height();
            actors.push_back(actor);
        }
    }

    return MergeActors(actors);
}