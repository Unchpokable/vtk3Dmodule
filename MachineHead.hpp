#pragma once

#include "pch.h"
#include "MachineHeadAssembly.hpp"
#include "ProbeToolAssembly.hpp"

class MachineHead
{
public:
    MachineHead(const MachinePartCollection& parts)
    {
        for (const auto& part: parts)
        {
            _topLevelParts.emplace_back(part);
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

    void RotatePart(RotAddress address, double angle) 
    {
        for (const auto part : _topLevelParts) 
        {
            part.Rotate(address, angle);
        }
    }

    void AddStylus(const ProbeToolAssembly& tool) 
    {

    }

private:
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
