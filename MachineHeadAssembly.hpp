#pragma once

#include "pch.h"

#include "vtkUtils.hpp"
#include "xmltools.hpp"

class MachineHeadAssembly
{
public:
    MachineHeadAssembly(MachinePart part) : _part(std::move(part))
    {
        _assembly = LoadObj(_part.Models());

        for(const auto child: _part.ChildMachines())
        {
            _childElements.push_back(new MachineHeadAssembly(*child));
        }
    }

    void Rotate(RotAddress address, double angle) const
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
        return _assembly;
    }

    std::vector<MachineHeadAssembly*> ChildElements() const
    {
        return _childElements;
    }

private:
    MachinePart _part;
    vtkActorPointer _assembly;

    std::vector<MachineHeadAssembly*> _childElements;

    void RotateImpl(const MachineHeadAssembly* target, RotAddress address, double angle) const
    {
        // uga-buga code moment
        if(_part.CanRotate() && _part.RotationAxisAddress() == address)
        {
            const auto axis = target->_part.Axis().Axis();

            const auto offset = Eigen::Vector3d(axis.X, axis.Y, axis.Z);
            const auto rotation = Eigen::Vector3d(axis.I, axis.J, axis.K);

            auto affineTransform = Eigen::Affine3d::Identity();
            affineTransform.translate(offset);

            affineTransform.rotate(Eigen::AngleAxisd(angle * M_PI / 180, rotation.normalized()));

            target->ApplyAffine(affineTransform);
            target->ForceRotateChildren(affineTransform);
        }

        if (!target->_childElements.empty())
        {
            for (const auto child: _childElements)
            {
                RotateImpl(child, address, angle);
            }
        }
    }

    void ApplyAffine(const Eigen::Affine3d& transform) const
    {
        auto matrix = transform.matrix();

        const vtkNew<vtkMatrix4x4> vtkMat {};

        for(int i = 0; i < 4; ++i) {
            for(int j = 0; j < 4; ++j) {
                vtkMat->SetElement(i, j, matrix(i, j));
            }
        }

        _assembly->SetUserMatrix(vtkMat);
    }

    void ForceRotateChildren(const Eigen::Affine3d& transform) const
    {
        for (const auto child: _childElements)
        {
            child->ApplyAffine(transform);
            child->ForceRotateChildren(transform);
        }
    }

    static vtkActorPointer LoadObj(const MachineModelList& models)
    {
        const auto reader = vtkSmartPointer<vtkOBJReader>::New();

        std::vector<vtkSmartPointer<vtkActor>> result;

        for (const auto& obj: models.Parts())
        {
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
