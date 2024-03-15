#pragma once

#include "pch.h"

#include "ProbeToolAssembly.hpp"
#include "vtkUtils.hpp"
#include "xmltools.hpp"

class MachineHeadAssembly
{
    friend class MachineHead;

public:
    MachineHeadAssembly(MachinePart* part, MachineHeadAssembly* parent = nullptr);

    void SetPartRotation(RotAddress address, double angle) const;

    bool HasRotation() const;

    RotAddress RotationAddress() const;

    vtkActorPointer Actor() const
    {
        return _actor;
    }

    std::vector<MachineHeadAssembly*> ChildElements() const
    {
        return _childElements;
    }

    void InsertChild(MachineHeadAssembly* child);

    const MachineHeadAssembly* LastChild() const;

    const MachineHeadAssembly* Parent() const
    {
        return _parent;
    }

    void Restore() const;

    void UpdateChild() const;

    void SetBaseRZ(const vtkSmartPointer<vtkTransform>& rz, bool isChild = false);

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

    void RotateImpl(RotAddress address, double angle) const;

    void TransformChildren(const vtkSmartPointer<vtkTransform>& transform) const;

    void AppendMesh(const vtkActorPointer& object);

    static vtkActorPointer LoadObj(const MachineModelList& models);
};
