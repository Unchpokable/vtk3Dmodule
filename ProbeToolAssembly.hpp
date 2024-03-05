#pragma once

#include "generators.hpp"
#include "xmltools.hpp"
#include "vtkUtils.hpp"

class ProbeToolAssembly
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    vtkActorPointer Weld() const
    {
        return MergeActors(_parts, false);
    }

    void Build(const ProbePartCatalog& catalog, const CatalogAssembly& assembly)
    {
        Build(catalog, assembly.Elements());
    }

    void Build(const ProbePartCatalog& catalog, const std::vector<std::string>& names)
    {
        for (const auto& name: names)
        {
            AddPart(catalog, name);
        }
    }

    void AddPart(const ProbePartCatalog& catalog, const std::string& partName)
    {
        const auto part = catalog.FindByName(partName);
        if (part)
            AddPart(*part);
    }

    void AddPart(const SimpleProbePart& part)
    {
        const auto actor = GeneratePolyPart(part.Geometry());
        actor->SetPosition(0, _toolLength, 0);
        _toolLength += part.Length();

        _parts.push_back(actor);
    }

    std::vector<vtkActorPointer> Parts() const noexcept
    {
        return _parts;
    }

    double Length() const noexcept
    {
        return _toolLength;
    }

private:

    double _toolLength = 0;
    std::vector<vtkActorPointer> _parts;

    std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d>> _stylusesPoints; //TODO
};