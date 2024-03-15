#include "MachineHeadAssembly.hpp"

MachineHeadAssembly::MachineHeadAssembly(MachinePart* part, MachineHeadAssembly* parent) : _part(part)
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
    for (const auto child : _part->ChildMachines())
    {
        _childElements.push_back(new MachineHeadAssembly(child, this));
    }
}

void MachineHeadAssembly::SetPartRotation(RotAddress address, double angle) const 
{
    if (_part)
        RotateImpl(address, angle);
}

bool MachineHeadAssembly::HasRotation() const 
{
    if (_part)
        return _part->CanRotate();
    return false;
}

RotAddress MachineHeadAssembly::RotationAddress() const 
{
    if (_part)
        return _part->RotationAxisAddress();
    return RotAddress::None;
}

void MachineHeadAssembly::InsertChild(MachineHeadAssembly* child) 
{
    if (child)
        _childElements.push_back(child);
}

const MachineHeadAssembly* MachineHeadAssembly::LastChild() const 
{
    if (!_childElements.empty())
        return _childElements.back()->LastChild();

    return this;
}

void MachineHeadAssembly::Restore() const 
{
    const auto newActor = LoadObj(_part->Models());
    _actor->SetMapper(newActor->GetMapper());
}

void MachineHeadAssembly::UpdateChild() const
{
    TransformChildren(_transform);
}

void MachineHeadAssembly::SetBaseRZ(const vtkSmartPointer<vtkTransform>& rz, bool isChild) 
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

//------ privates --------

void MachineHeadAssembly::RotateImpl(RotAddress address, double angle) const 
{
    if (_part->CanRotate() && _part->RotationAxisAddress() == address)
    {
        const auto axis = _part->Axis().Axis();

        _transform->Identity();
        _transform->PreMultiply();

        if (_rzRotation && _rzRotation.GetPointer())
            _transform->Concatenate(GetRZ());

        _transform->Translate(axis.X, axis.Y, axis.Z);
        _transform->RotateWXYZ(angle, axis.I, axis.J, axis.K);

        TransformChildren(_transform);
    }

    else if (!_childElements.empty())
    {
        for (const auto child : _childElements)
        {
            child->RotateImpl(address, angle);
        }
    }
}

void MachineHeadAssembly::TransformChildren(const vtkSmartPointer<vtkTransform>& transform) const 
{
    for (const auto child : _childElements)
    {
        child->_transform->PostMultiply();
        child->_transform->Concatenate(transform);
        child->_transform->PreMultiply();
        child->TransformChildren(child->_transform);
    }
}

void MachineHeadAssembly::AppendMesh(const vtkActorPointer& object) 
{
    auto transform = vtkTransform::SafeDownCast(object->GetUserTransform());
    transform->PostMultiply();
    transform->Concatenate(_actor->GetUserTransform());
    transform->PreMultiply();
    const auto newActor = MergeActors({ _actor, object }, false);
    _actor->SetMapper(newActor->GetMapper());
    _actor->SetUserTransform(_transform);
}

vtkActorPointer MachineHeadAssembly::LoadObj(const MachineModelList& models)
{
    std::vector<vtkSmartPointer<vtkActor>> result;

    for (const auto& obj : models.Parts())
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