#pragma once

#include "pch.h"

#include "vtkUtils.hpp"
#include "xmltools.hpp"

class MachineHeadAssembly
{
public:
    MachineHeadAssembly(const MachinePart& part) : _part(part)
    {
        _assembly = vtkSmartPointer<vtkActor>::New();
    }

    void RotateA(const Eigen::Affine3d& rotation)
    {
        
    }

    void RotateB(const Eigen::Affine3d& rotation)
    {
        
    }

private:
    MachinePart _part;
    vtkSmartPointer<vtkActor> _assembly;

    std::vector<MachineHeadAssembly*> _childElements;

    void LoadObj()
    {
        const auto reader = vtkSmartPointer<vtkOBJReader>::New();

        std::vector<vtkSmartPointer<vtkActor>> result;

        for (const auto& obj: _part.Models().Parts())
        {
            reader->SetFileName(obj.MeshFile().toStdString().c_str());
            reader->Update();

            const auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputConnection(reader->GetOutputPort());

            const auto part = vtkSmartPointer<vtkActor>::New();
            part->SetMapper(mapper);

            result.push_back(part);
        }

        _assembly = MergeActors(result);
    }
};
