#pragma once

#include <type_traits>

#include "result.hpp"

using vtkRendererPointer = vtkSmartPointer<vtkRenderer>;
using vtkActorPointer = vtkSmartPointer<vtkActor>;
using vtkTexturePointer = vtkSmartPointer<vtkTexture>;

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

template<typename ActorPtr, std::enable_if_t<std::is_convertible_v<ActorPtr, vtkProp*>, bool> = true>
Status AddActorsToRenderer(const vtkRendererPointer& renderer, const std::vector<ActorPtr>& actors)
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

inline Result<vtkTexturePointer> LoadVtkTexture(const std::string& path) {
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