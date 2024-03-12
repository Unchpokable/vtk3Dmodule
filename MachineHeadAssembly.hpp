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
            RotateImpl(this, address, angle);
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

    void Restore()
    {
        if (_rzRotation && _rzRotation.GetPointer())
            SetBaseRZ(_rzRotation);
        //TODO Check memory usage here
        const auto newActor = LoadObj(_part->Models());
        _actor->SetMapper(newActor->GetMapper());

        if(_parent != nullptr)
        {
            _transform->PostMultiply();
            _transform->SetInput(_parent->_transform);
        }

        _actor->SetUserTransform(_transform);
    }

    void UpdateChild() 
    {
        TransformChildren(_transform);
    }

    void SetBaseRZ(const vtkSmartPointer<vtkTransform> rz) 
    {
        _transform->PostMultiply();
        _transform->SetInput(rz);
        _actor->SetUserTransform(_transform);

        _rzRotation = rz;

        for (const auto child : _childElements) 
        {
            child->SetBaseRZ(rz);
        }
    }

    void SetOrientation(double x, double y, double z) 
    {
        _actor->SetOrientation(x, y, z);

        for (const auto child : _childElements)
        {
            child->SetOrientation(x, y, z);
        }
    }

private:
    MachinePart* _part;
    vtkActorPointer _actor;

    vtkTransform* _transform;
    vtkSmartPointer<vtkTransform> _rzRotation;
    std::vector<MachineHeadAssembly*> _childElements;
    MachineHeadAssembly* _parent;

    void RotateImpl(const MachineHeadAssembly* target, RotAddress address, double angle) const
    {
        // not uga-buga code
        if(target->_part->CanRotate() && target->_part->RotationAxisAddress() == address)
        {
            const auto axis = target->_part->Axis().Axis();

            target->_transform->Identity();
            target->_transform->PostMultiply();

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
