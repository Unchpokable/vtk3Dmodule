#pragma once

#include "generators.hpp"
#include "xmltools.hpp"
#include "vtkUtils.hpp"

class ProbeToolAssembly
{
public:
    vtkActorPointer Weld() const
    {
        return MergeActors(_parts, false);
    }

    void Build(const ProbePartCatalog& catalog, const CatalogAssembly* const assembly)
    {
        if (!assembly)
        {
            qDebug() << "Assembly pointer was nullptr";
            return;
        }
        Build(catalog, *assembly);
    }

    void Build(const ProbePartCatalog& catalog, const CatalogAssembly& assembly)
    {
        Build(catalog, assembly.Elements());
    }

    void Build(const ProbePartCatalog& catalog, const std::vector<std::string>& names)
    {
        for (const auto& name: names)
        {
            AddPartFromCatalog(catalog, name);
        }
    }

    void AddPart(const SimpleProbePart* const part)
    {
        const auto actor = GeneratePolyPart(part->Geometry());
        actor->SetPosition(0, _toolLength, 0);
        _toolLength += part->Length();
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

    void AddPartFromCatalog(const ProbePartCatalog& catalog, const std::string& name)
    {
        const auto part = catalog.FindByName(name);

        if(part)
            AddPart(part);
    }

    double _toolLength = 0;
    std::vector<vtkActorPointer> _parts;
};