#pragma once

#include "pch.h"

#include "ProbeToolAssembly.hpp"
#include "vtkUtils.hpp"
#include "xmltools.hpp"

class MachineHeadAssembly
{
    friend class MachineHead;

public:
    MachineHeadAssembly(MachinePart* part, MachineHeadAssembly* parent = nullptr) : _part(part)
    {
        _actor = LoadObj(_part->Models());
        _parent = parent;
        const auto transform = vtkSmartPointer<vtkTransform>::New();

        if (_parent != nullptr)
        {
            transform->PostMultiply();
            transform->SetInput(_parent->_transform);
        }

        _actor->SetUserTransform(transform);

        _transform = vtkTransform::SafeDownCast(_actor->GetUserTransform());
        for(const auto child: _part->ChildMachines())
        {
            _childElements.push_back(new MachineHeadAssembly(child, this));
        }
    }

    void SetPartRotation(RotAddress address, double angle) const
    {
        if (_part)
            RotateImpl(address, angle);
    }

    bool HasRotation() const 
    {
        if (_part)
            return _part->CanRotate();
        return false;
    }

    RotAddress RotationAddress() const
    {
        if (_part)
            return _part->RotationAxisAddress();
        return RotAddress::None;
    }

    vtkActorPointer Actor() const
    {
        return _actor;
    }

    std::vector<MachineHeadAssembly*> ChildElements() const
    {
        return _childElements;
    }

    void InsertChild(MachineHeadAssembly* child)
    {
        if (child)
            _childElements.push_back(child);
    }

    const MachineHeadAssembly* LastChild() const
    {
        if(!_childElements.empty())
            return _childElements.back()->LastChild();

        return this;
    }

    const MachineHeadAssembly* Parent() const
    {
        return _parent;
    }

    void Restore() const
    {
        const auto newActor = LoadObj(_part->Models());
        _actor->SetMapper(newActor->GetMapper());
    }

    void UpdateChild() const
    {
        TransformChildren(_transform);
    }

    void SetBaseRZ(const vtkSmartPointer<vtkTransform>& rz, bool isChild = false) 
    {
        _transform->DeepCopy(rz);
        _actor->SetUserTransform(_transform);

        if (!isChild)
            _rzRotation = rz;

        for (const auto child : _childElements)
        {
            child->SetBaseRZ(rz, true);
        }
    }

    vtkSmartPointer<vtkTransform> GetRZ() const
    {
        return _rzRotation;
    }

private:
    MachinePart* _part;
    vtkActorPointer _actor;

    vtkTransform* _transform;
    vtkSmartPointer<vtkTransform> _rzRotation;
    std::vector<MachineHeadAssembly*> _childElements;
    MachineHeadAssembly* _parent;

    void RotateImpl(RotAddress address, double angle) const
    {
        if(_part->CanRotate() && _part->RotationAxisAddress() == address) 
        {
            const auto axis = _part->Axis().Axis();

            _transform->Identity();
            _transform->PreMultiply();

            if(_rzRotation && _rzRotation.GetPointer())
                _transform->Concatenate(GetRZ());

            _transform->Translate(axis.X, axis.Y, axis.Z);
            _transform->RotateWXYZ(angle, axis.I, axis.J, axis.K);

            TransformChildren(_transform);
        }

        else if(!_childElements.empty()) 
        {
            for(const auto child : _childElements) 
            {
                child->RotateImpl(address, angle);
            }
        }
    }

    void TransformChildren(const vtkSmartPointer<vtkTransform>& transform) const
    {
        for(const auto child: _childElements)
        {
            child->_transform->PostMultiply();
            child->_transform->Concatenate(transform);
            child->_transform->PreMultiply();
            child->TransformChildren(child->_transform);
        }
    }

    void AppendMesh(const vtkActorPointer& object)
    {
        vtkTransform::SafeDownCast(object->GetUserTransform())->PostMultiply();
        vtkTransform::SafeDownCast(object->GetUserTransform())->SetInput(_actor->GetUserTransform());
        const auto newActor = MergeActors({ _actor, object }, false);
        _actor->SetMapper(newActor->GetMapper());
        _actor->SetUserTransform(_transform);
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
