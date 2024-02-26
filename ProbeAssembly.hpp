#pragma once

#include "pch.h"

#include "xmltools.hpp"

class ProbeAssembly
{
public:
    ProbeAssembly(const MachinePart& part) : _part(part)
    {
        _assembly = vtkSmartPointer<vtkAssembly>::New();
    }

    void Rotate(const Eigen::Affine3d& rotation)
    {
        
    }

private:
    MachinePart _part;
    vtkSmartPointer<vtkAssembly> _assembly;

    std::vector<ProbeAssembly*> _childElements;

    void LoadObj()
    {
        const auto reader = vtkSmartPointer<vtkOBJReader>::New();

        for (const auto& obj: _part.Models().Parts())
        {
            reader->SetFileName(obj.MeshFile().toStdString().c_str());
            reader->Update();

            const auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputConnection(reader->GetOutputPort());

            const auto part = vtkSmartPointer<vtkActor>::New();
            part->SetMapper(mapper);

            _assembly->AddPart(part);
        }
    }
};
