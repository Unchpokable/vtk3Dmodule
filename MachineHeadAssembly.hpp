#pragma once

#include "pch.h"

#include "vtkUtils.hpp"
#include "xmltools.hpp"

class MachineHeadAssembly
{
public:
    MachineHeadAssembly(MachinePart part, MachineHeadAssembly* parent = nullptr) : _part(std::move(part))
    {
        _actor = LoadObj(_part.Models());
        _parent = parent;
        _transform = vtkSmartPointer<vtkTransform>::New();

        if(_parent != nullptr)
            _transform->PostMultiply();

        _actor->SetUserTransform(_transform);
        for(const auto child: _part.ChildMachines())
        {
            _childElements.push_back(new MachineHeadAssembly(*child, this));
        }
    }

    void SetPartRotation(RotAddress address, double angle) const
    {
        RotateImpl(this, address, angle);
    }

    bool HasRotation() const 
    {
        return _part.CanRotate();
    }

    RotAddress RotationAddress() const
    {
        return _part.RotationAxisAddress();
    }

    vtkActorPointer Actor() const
    {
        return _actor;
    }

    std::vector<MachineHeadAssembly*> ChildElements() const
    {
        return _childElements;
    }

private:
    MachinePart _part;
    vtkActorPointer _actor;

    vtkSmartPointer<vtkTransform> _transform;
    std::vector<MachineHeadAssembly*> _childElements;
    MachineHeadAssembly* _parent;

    void RotateImpl(const MachineHeadAssembly* target, RotAddress address, double angle) const
    {
        // not uga-buga code
        if(target->_part.CanRotate() && target->_part.RotationAxisAddress() == address)
        {
            const auto axis = target->_part.Axis().Axis();

            target->_transform->Identity();
            target->_transform->Translate(axis.X, axis.Y, axis.Z);
            target->_transform->RotateWXYZ(angle, axis.I, axis.J, axis.K);

            target->TransformChildren(target->_transform);
        }

        else if (!target->_childElements.empty())
        {
            for (const auto child: target->_childElements)
            {
                RotateImpl(child, address, angle);
            }
        }
    }

    void TransformChildren(const vtkSmartPointer<vtkTransform>& transform) const
    {
        for(const auto child: _childElements)
        {
            child->_transform->Concatenate(transform);
            child->TransformChildren(transform);
        }
    }

    static vtkActorPointer LoadObj(const MachineModelList& models)
    {
        std::vector<vtkSmartPointer<vtkActor>> result;

        for (const auto& obj: models.Parts())
        {
            const auto reader = vtkSmartPointer<vtkOBJReader>::New();
            reader->SetFileName(obj.MeshFile().toStdString().c_str());
            reader->Update();

            const auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputConnection(reader->GetOutputPort());

            const auto part = vtkSmartPointer<vtkActor>::New();
            part->SetMapper(mapper);
            const auto color = obj.ColorF();
            part->GetProperty()->SetColor(color.R(), color.G(), color.B());
            result.push_back(part);
        }

        return MergeActors(result);
    }
};
