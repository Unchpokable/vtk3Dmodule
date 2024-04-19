#pragma once

#include "pch.h"

#include <type_traits>
#include "result.hpp"


using vtkRendererPointer = vtkSmartPointer<vtkRenderer>;
using vtkActorPointer = vtkSmartPointer<vtkActor>;
using vtkTexturePointer = vtkSmartPointer<vtkTexture>;
using vtkPolyDataMapperPointer = vtkSmartPointer<vtkPolyDataMapper>;

using vtkActorTransformAction = std::function<vtkActor* (vtkActor*)>;
using vtkActorTransformActionVoid = std::function<void(vtkActor*)>;

template<typename ...Props>
using is_vtk_prop_ptr = std::conjunction<std::is_convertible<Props, vtkProp*>...>;

template<typename ...Actors,
    std::enable_if_t<is_vtk_prop_ptr<Actors...>::value, bool> = true>
void AddActorsToRenderer(const vtkRendererPointer& renderer, Actors... actors)
{
    if(!renderer.GetPointer())
        return;
    (renderer->AddActor(actors), ...);
}

inline void AddActorsToRenderer(const vtkRendererPointer& renderer, vtkActorCollection* actors)
{
    actors->InitTraversal();

    auto actor = actors->GetNextActor();
    while(actor)
    {
        renderer->AddActor(actor);

        actor = actors->GetNextActor();
    }
}


//std::enable_if_t<std::is_convertible_v<ActorPtr, vtkProp*>, bool> = true

template<typename ActorPtr, template<class, class = std::allocator<ActorPtr>> class X, std::enable_if_t<std::is_convertible_v<ActorPtr, vtkProp*>, bool> = true>
Status AddActorsToRenderer(const vtkRendererPointer& renderer, const X<ActorPtr>& actors)
{
    if(!renderer.GetPointer())
        return Status::NullArgument;

    for(const auto& actor : actors)
    {
        if(!actor.GetPointer())
            return Status::NullArgument;
        renderer->AddActor(actor);
    }

    return Status::Success;
}

inline void TransformRendererActors(const vtkRendererPointer& renderer, const vtkActorTransformActionVoid& transform)
{
    auto actors = renderer->GetActors();

    actors->InitTraversal();

    auto actor = actors->GetNextActor();

    while (actor)
    {
        transform(actor);
        actor = actors->GetNextActor();
    }
}

inline void SetActorsToWireframeDisplay(const vtkRendererPointer& renderer)
{
    TransformRendererActors(renderer, [](vtkActor* actor) {
        actor->GetProperty()->SetRepresentationToWireframe();
    });
}

inline void SetActorsToSurfaceDisplay(const vtkRendererPointer& renderer)
{
    TransformRendererActors(renderer, [](vtkActor* actor) {
        actor->GetProperty()->SetRepresentationToSurface();
    });
}

inline Result<vtkTexturePointer> LoadVtkTexture(const std::string& path)
{
    const auto reader = vtkSmartPointer<vtkPNGReader>::New();

    if(!reader->CanReadFile(path.c_str()))
        return { Status::BadArgument, "Given file can not be read" };

    reader->SetFileName(path.c_str());
    reader->Update();

    auto texture = vtkTexturePointer::New();
    texture->SetInputConnection(reader->GetOutputPort());
    texture->InterpolateOff();

    return texture; //Implicit conversion - Real return type is Result<vtkSmartPointer<vtkTexture>>
}

/**
 * \brief Merges all given actors to single one \c vtkActor, saving colors and mesh geometry and positions.
 * \param actors \c std::vector with actors to be merged
 * \param useActorPropertyToExtractColor \c bool flag defines that function should use PolyData or property to copy color of source actors
 * \return result \c vtkSmartPointer to \c vtkActor
 */
inline vtkActorPointer MergeActors(const std::vector<vtkActorPointer>& actors, bool useActorPropertyToExtractColor = true)
{
    const vtkNew<vtkPolyDataMapper> finalMapper;
    const vtkNew<vtkAppendPolyData> appendData;
    const vtkNew<vtkUnsignedCharArray> colors;

    colors->SetName("Colors");
    colors->SetNumberOfComponents(3);

    for (const auto& actor: actors)
    {
        const auto mapperData = vtkPolyDataMapper::SafeDownCast(actor->GetMapper())->GetInput();

        vtkNew<vtkTransform> transform;

        transform->SetMatrix(actor->GetMatrix());

        const vtkNew<vtkTransformPolyDataFilter> filter;
        filter->SetTransform(transform);
        filter->SetInputData(mapperData);
        filter->Update();

        appendData->AddInputData(filter->GetOutput());

        if (useActorPropertyToExtractColor)
        {
            const auto color = actor->GetProperty()->GetColor();
            const double colorUC[3] = {
                color[0] * 255,
                color[1] * 255,
                color[2] * 255
            };

            const auto numPoints = filter->GetOutput()->GetNumberOfPoints();
            for(vtkIdType i = 0; i < numPoints; ++i) {
                colors->InsertNextTuple(colorUC);
            }
        }
        else
        {
            const auto scalars = mapperData->GetPointData()->GetScalars();
            if (scalars)
            {
                for (vtkIdType i = 0; i < scalars->GetNumberOfTuples(); i++)
                {
                    double color[3];
                    scalars->GetTuple(i, color);
                    colors->InsertNextTuple(color);
                }
            }
        }
    }

    appendData->Update();

    const auto combinedPolyData = appendData->GetOutput();
    combinedPolyData->GetPointData()->SetScalars(colors);

    finalMapper->SetInputConnection(appendData->GetOutputPort());
    finalMapper->SetInputData(combinedPolyData);


    auto result = vtkActorPointer::New();
    result->SetMapper(finalMapper);
    return result;
}

inline vtkSmartPointer<vtkTexture> CreateTextureFromQWidget(QWidget* target)
{
    const auto widgetRepr = vtkSmartPointer<vtkQWidgetRepresentation>::New();
    widgetRepr->SetWidget(target);

    const auto texture = vtkSmartPointer<vtkOpenGLTexture>::New();
    texture->SetTextureObject(widgetRepr->GetQWidgetTexture());

    return texture;
}

inline vtkSmartPointer<vtkTexturedActor2D> CreateTexturedActorFromQWidget(QWidget* target)
{
    const auto texture = CreateTextureFromQWidget(target);

    auto actor2d = vtkSmartPointer<vtkTexturedActor2D>::New();
    const auto mapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();

    const vtkNew<vtkFloatArray> tcoords;
    tcoords->SetNumberOfComponents(2);
    tcoords->InsertNextTuple2(0.f, 0.0f);
    tcoords->InsertNextTuple2(0.0f, 1.0f);
    tcoords->InsertNextTuple2(1.0f, 1.0f);
    tcoords->InsertNextTuple2(1.0f, 0.0f);

    const vtkNew<vtkPolyData> bgData;
    const vtkNew<vtkPoints> points;
    const vtkNew<vtkCellArray> cells;
    vtkIdType pts[4];
    pts[0] = points->InsertNextPoint(0, 0, 0);
    pts[1] = points->InsertNextPoint(0, target->height(), 0);
    pts[2] = points->InsertNextPoint(target->width(), target->height(), 0);
    pts[3] = points->InsertNextPoint(target->width(), 0, 0);
    bgData->SetPoints(points);
    cells->InsertNextCell(4, pts);
    bgData->SetPolys(cells);
    bgData->GetPointData()->SetTCoords(tcoords);
    mapper->SetInputData(bgData);
    actor2d->SetMapper(mapper);
    actor2d->SetTexture(texture);

    return actor2d;
}

// Kinda shader config

inline void SetActorLightingMetallic(const vtkActorPointer& actor, bool usePBR = true)
{
    // Should work cool with PBR
    const auto property = actor->GetProperty();
    if (usePBR)
    {
        property->SetInterpolationToPBR();
        property->SetMetallic(1);
        property->SetRoughness(0.5);
    }
    else
    {
        property->SetSpecular(.1);
        property->SetSpecularPower(80);
        property->SetAmbient(.4);
        property->SetDiffuse(.3);
    }
}

inline void SetActorLightingPlastic(const vtkActorPointer& actor)
{
    const auto property = actor->GetProperty();

    property->SetSpecular(.6);
    property->SetSpecularPower(20);
    property->SetAmbient(.4);
    property->SetDiffuse(.6);
}

inline void SetActorLightingMatte(const vtkActorPointer& actor)
{
    const auto property = actor->GetProperty();

    property->SetSpecular(.1);
    property->SetSpecularPower(5);
    property->SetAmbient(.4);
    property->SetDiffuse(.8);
}