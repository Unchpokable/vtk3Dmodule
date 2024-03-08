#pragma once

#include "pch.h"
#include "MachineHeadAssembly.hpp"
#include "ProbeToolAssembly.hpp"

class MachineHead
{
public:
    MachineHead(const MachinePartCollection& parts) : _parts(parts)
    {
        for (auto& part: _parts)
        {
            _topLevelParts.emplace_back(&part, nullptr);
        }
    }

    std::vector<vtkActorPointer> Actors() const
    {
        std::vector<vtkActorPointer> result;

        for (const auto& assembly: _topLevelParts)
        {
            const auto assemblyActors = GetActors(&assembly);
            result.insert(result.end(), assemblyActors.begin(), assemblyActors.end());
        }

        return result;
    }

    void RotatePart(RotAddress address, double angle) const
    {
        for (const auto& part : _topLevelParts) 
        {
            part.SetPartRotation(address, angle);
        }
    }

    void AddStylus(const ProbeToolAssembly& tool) const
    {
        const auto lastChild = const_cast<MachineHeadAssembly*>(_topLevelParts.back().LastChild());

        const vtkNew<vtkTransform> transform;
        transform->RotateWXYZ(-90, 1, 0, 0);
        transform->Translate(0, 37, 0);

        const auto actor = tool.Weld();
        actor->SetUserTransform(transform);

        //lastChild->AppendMesh(actor);

        const_cast<MachineHeadAssembly*>(lastChild->Parent())->InsertChild(new MachineHeadAssembly(actor, lastChild));
    }

private:
    MachinePartCollection _parts;
    std::vector<MachineHeadAssembly> _topLevelParts;

    static std::vector<vtkActorPointer> GetActors(const MachineHeadAssembly* assembly)
    {
        std::vector<vtkActorPointer> result;

        result.push_back(assembly->Actor());

        const auto childElements = assembly->ChildElements();
        for (const auto child: childElements)
        {
            // :thumbsup:
            const auto childActors = GetActors(child);
            result.insert(result.end(), childActors.begin(), childActors.end());
        }

        return result;
    }
};
