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

    void RotatePart(RotAddress address, double angle)
    {
        _rotations[address] = angle;

        for (const auto& part : _topLevelParts) 
        {
            part.SetPartRotation(address, angle);
        }
    }

    void RotateRZ(double angle, const Eigen::Vector3d& rzAxis)
    {
        if (_topLevelParts.empty())
            return;

        /*const auto transform = vtkSmartPointer<vtkTransform>::New();
        transform->RotateWXYZ(angle, rzAxis.x(), rzAxis.y(), rzAxis.z());
        
        vtkTransform::SafeDownCast(_topLevelParts[0].Actor()->GetUserTransform())->PostMultiply();
        vtkTransform::SafeDownCast(_topLevelParts[0].Actor()->GetUserTransform())->SetInput(transform);*/

        for (std::size_t i = 0; i < _topLevelParts.size(); i++) 
        {
            /*auto actorTransform = vtkTransform::SafeDownCast(_topLevelParts[i].Actor()->GetUserTransform());
            actorTransform->PostMultiply();
            actorTransform->SetInput(_topLevelParts[i - 1].Actor()->GetUserTransform());
            _topLevelParts[i].SetBaseRZ(transform);*/

            _topLevelParts[i].SetOrientation(rzAxis.x() * angle, rzAxis.y() * angle, rzAxis.z() * angle);
        }
    }

    void AddStylus(const ProbeToolAssembly& tool) const
    {
        RemoveStylus();
        const auto lastChild = const_cast<MachineHeadAssembly*>(_topLevelParts.back().LastChild());

        const vtkNew<vtkTransform> transform;

        //TODO Fix magic numbers in translate
        transform->RotateWXYZ(-90, 1, 0, 0);
        transform->Translate(0, 37, 0);

        const auto actor = tool.Weld();
        actor->SetUserTransform(transform);
        lastChild->Restore();
        lastChild->AppendMesh(actor);

        //const_cast<MachineHeadAssembly*>(lastChild->Parent())->InsertChild(new MachineHeadAssembly(actor, lastChild));
    }

    void RemoveStylus() const
    {
        const auto lastChild = const_cast<MachineHeadAssembly*>(_topLevelParts.back().LastChild());
        lastChild->Restore();
    }

    double GetRotation(RotAddress address) const
    {
        return _rotations.at(address);
    }

private:
    MachinePartCollection _parts;
    std::vector<MachineHeadAssembly> _topLevelParts;

    std::map<RotAddress, double> _rotations = 
    {
        {RotAddress::A, 0},
        {RotAddress::B, 0}
    };

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
