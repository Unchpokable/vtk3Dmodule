#pragma once

#include "pch.h"
#include "RgbaF.hpp"

//TODO: Need refactoring

using byte = unsigned char;

class RotationAddress : QObject
{
    Q_OBJECT
    Q_ENUMS(Address)

public:
    explicit RotationAddress() = default; 

    enum Address
    {
        A, B, None
    };

    static Address FromString(const QString& str)
    {
        const int index = staticMetaObject.indexOfEnumerator("Address");
        const auto metaEnum = staticMetaObject.enumerator(index);

        return static_cast<Address>(metaEnum.keyToValue(str.toStdString().c_str()));
    }

    static QString ToString(Address val)
    {
        const int index = staticMetaObject.indexOfEnumerator("Address");
        const auto metaEnum = staticMetaObject.enumerator(index);

        return metaEnum.valueToKey(val);
    }
};

using RotAddress = RotationAddress::Address;

enum class GeometryType
{
    Cylinder,
    Cone,
    Sphere
};

inline GeometryType GeometryTypeFromString(const std::string& string) noexcept
{
    if(string == "Cylinder")
        return GeometryType::Cylinder;

    if(string == "Cone")
        return GeometryType::Cone;

    if(string == "Sphere")
        return GeometryType::Sphere;

    return GeometryType::Cylinder;
}

enum class ProbePartType
{
    Connector,
    Extension,
    Module,
    Stylus
};

inline ProbePartType ProbePartTypeFromString(const std::string& string) noexcept
{
    if(string == "Connector")
        return ProbePartType::Connector;

    if(string == "Extension")
        return ProbePartType::Extension;

    if(string == "Stylus")
        return ProbePartType::Stylus;

    if(string == "Module")
        return ProbePartType::Module;

    return ProbePartType::Connector;
}


class ProbeHeadExtensionGeometryData
{
public:
    static ProbeHeadExtensionGeometryData FromXML(const QDomElement& xmlDom) noexcept
    {
        ProbeHeadExtensionGeometryData out = GeometryTypeFromString(xmlDom.tagName().toStdString());

        out._height = xmlDom.attribute("height").toDouble();
        out._diameter = xmlDom.attribute("diameter").toDouble();
        out._diameter1 = xmlDom.attribute("diameter1").toDouble();
        out._diameter2 = xmlDom.attribute("diameter2").toDouble();
        out._offset = xmlDom.attribute("offset").toDouble();
        out._reference_length = xmlDom.attribute("reference_length").toDouble();
        out._material = xmlDom.attribute("material");
        out._hexColor = xmlDom.attribute("color");

        return out;
    }

    GeometryType Type() const noexcept
    {
        return _type;
    }

    double Height() const noexcept
    {
        return _height;
    }

    double Diameter() const noexcept
    {
        return _diameter;
    }

    double Diameter1() const noexcept
    {
        return _diameter1;
    }

    double Diameter2() const noexcept
    {
        return _diameter2;
    }

    double Offset() const noexcept
    {
        return _offset;
    }

    double ReferenceLength() const noexcept
    {
        return _reference_length;
    }

    QString Material() const noexcept
    {
        return _material;
    }

    QString HexColor() const noexcept
    {
        return _hexColor;
    }

private:
    ProbeHeadExtensionGeometryData(GeometryType type) : _type(type) {}

    GeometryType _type;

    double _height = 0;
    double _diameter = 0;
    double _diameter1 = 0;
    double _diameter2 = 0;
    double _offset = 0;
    double _reference_length = 0;
    QString _material = "";
    QString _hexColor = "";
};

using GeometryDataContainer = std::vector<ProbeHeadExtensionGeometryData>;

class SimpleProbePart
{
public:
    explicit SimpleProbePart(const QDomElement& xmlDom) noexcept
    {
        _type = ProbePartTypeFromString(xmlDom.tagName().toStdString());
        _length = xmlDom.attribute("length").toDouble();
        _price = xmlDom.attribute("price");
        _name = xmlDom.attribute("name");
        _manufacturer = xmlDom.firstChildElement("Manufacturer").text();

        const auto nodes = xmlDom.firstChildElement("Geometry").childNodes();
        for(int i = 0; i < nodes.count(); ++i)
        {
            QDomNode node = nodes.item(i);
            if(node.isElement())
            {
                QDomElement element = node.toElement();
                _geometry.push_back(ProbeHeadExtensionGeometryData::FromXML(element));
            }
        }
    }

    QString Name() const noexcept
    {
        return _name;
    }

    QString Price() const noexcept
    {
        return _price;
    }

    double Length() const noexcept
    {
        return _length;
    }

    ProbePartType Type() const noexcept
    {
        return _type;
    }

    QString Manufacturer() const noexcept
    {
        return _manufacturer;
    }

    GeometryDataContainer Geometry() const noexcept
    {
        return _geometry;
    }

protected:
    ProbePartType _type;
    QString _manufacturer;
    GeometryDataContainer _geometry;

    QString _name;
    QString _price;
    double _length;
};

class Extension final : public SimpleProbePart
{
public:
    explicit Extension(const QDomElement& xmlElement) noexcept : SimpleProbePart(xmlElement)
    {
        _from_mounting = xmlElement.attribute("from_mounting");
        _to_mounting = xmlElement.attribute("to_mounting");
    }

    QString FromMounting() const noexcept
    {
        return _from_mounting;
    }

    QString ToMounting() const noexcept
    {
        return _to_mounting;
    }

private:
    QString _from_mounting;
    QString _to_mounting;
};

class Stylus final : public SimpleProbePart
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    explicit Stylus(const QDomElement& xmlElement) noexcept : SimpleProbePart(xmlElement)
    {
        _from_mounting = xmlElement.attribute("from_mounting");
        CalculateStylusEndPoint();
    }

    QString FromMounting()
    {
        return _from_mounting;
    }

    Eigen::Vector3d& StylusEnd()
    {
        return _stylusEnd;
    }

private:
    QString _from_mounting;
    Eigen::Vector3d _stylusEnd;

    void CalculateStylusEndPoint()
    {
        if(_geometry.empty())
            return;
        const auto sphere = *std::prev(_geometry.end());

        // "Diameter" from ProbePartCatalogue.xml looks like it is a radius
        const auto stylusEndY = Length() - sphere.Diameter();
        _stylusEnd = { 0, stylusEndY, 0 }; // VTK aligns any component along the Y axis by default so we needs only Y coordinate at this step
    }
};

class Module final : public SimpleProbePart
{
public:
    explicit Module(const QDomElement& xmlElement) noexcept : SimpleProbePart(xmlElement)
    {
        _dock_type = xmlElement.attribute("dock_type");
        _docking_height = xmlElement.attribute("docking_height");
    }

    QString DockType()
    {
        return _dock_type;
    }

    QString DockingHeight()
    {
        return _docking_height;
    }

private:
    QString _dock_type;
    QString _docking_height;
};

class ProbeBody final : public SimpleProbePart
{
public:
    explicit ProbeBody(const QDomElement& xmlElement) noexcept : SimpleProbePart(xmlElement)
    {
        _docking_height = xmlElement.attribute("docking_height");
    }

    QString DockingHeight() {
        return _docking_height;
    }

private:
    QString _docking_height;
};

class Connector : SimpleProbePart
{
    //TODO
};

class CatalogAssembly
{
public:
    using ElementsNamesCollection = std::vector<std::string>;

    CatalogAssembly(const QDomElement& xml) noexcept
    {
        const auto nodes = xml.childNodes();

        for (auto i = 0; i < nodes.count(); i++)
        {
            const auto elem = nodes.item(i).toElement();

            if(elem.tagName() == "Element")
                _elements.push_back(elem.attribute("ref").toStdString());
        }
    }

    ElementsNamesCollection Elements() const noexcept
    {
        return _elements;
    }

private:
    std::vector<std::string> _elements{};
};

class ProbePartCatalog {
public:
    template<typename T>
    using PartCollection = std::vector<T>;

    using AssembliesCollection = PartCollection<CatalogAssembly>;

    using CatalogFilter = std::function<bool(const QDomElement&)>;

    explicit ProbePartCatalog(const QString& path) noexcept
    {
        QDomDocument document;
        QFile file(path);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Failed to open the file for reading.";
        }

        if(!document.setContent(&file)) {
            qDebug() << "Failed to parse the file into a DOM tree.";
            file.close();
        }
        file.close();

        const auto catalog = document.firstChildElement("Catalogue");

        _parts = catalog.firstChildElement("ProbeParts");
        _assemblies = catalog.firstChildElement("Assemblies");
    }

    template<typename Part = SimpleProbePart,
        std::enable_if_t<std::is_base_of_v<SimpleProbePart, Part> || std::is_convertible_v<Part*, SimpleProbePart*>, bool> = true>
    Part* FindByName(const QString& name) const
    {
        const auto xmlElements = _parts.childNodes();

        for(int i = 0; i < xmlElements.count(); ++i) 
        {
            const auto elem = xmlElements.item(i).toElement();

            if(elem.hasAttribute("name"))
            {
                if(elem.attribute("name") == name)
                    return new Part(elem);
            }
        }

        return nullptr;
    }

    template<typename Part = SimpleProbePart,
        std::enable_if_t<std::is_base_of_v<SimpleProbePart, Part> || std::is_convertible_v<Part*, SimpleProbePart*>, bool> = true>
    Part* FindByName(const std::string& name) const
    {
        return FindByName<Part>(QString::fromStdString(name));
    }

    CatalogAssembly* FindAssembly(const QString& name) const
    {
        const auto xmlElements = _assemblies.childNodes();

        for(int i = 0; i < xmlElements.count(); ++i) {
            const auto elem = xmlElements.item(i).toElement();

            if(elem.hasAttribute("name")) {
                if(elem.attribute("name") == name)
                    return new CatalogAssembly(elem);
            }
        }

        return nullptr;
    }

    CatalogAssembly* FindAssembly(const std::string& name) const
    {
        return FindAssembly(QString::fromStdString(name));
    }

    AssembliesCollection Assemblies(const CatalogFilter& filter = nullptr) const
    {
        return BuildCollection<CatalogAssembly>(_assemblies, "Assembly", filter);
    }

    PartCollection<Extension> Extensions(const CatalogFilter& filter = nullptr) const
    {
        return BuildCollection<Extension>(_parts, "Extension", filter);
    }

    PartCollection<Stylus> Styluses(const CatalogFilter& filter = nullptr) const
    {
        return BuildCollection<Stylus>(_parts, "Stylus", filter);
    }

    PartCollection<ProbeBody> Probes(const CatalogFilter& filter = nullptr) const
    {
        return BuildCollection<ProbeBody>(_parts, "ProbeBody", filter);
    }

    PartCollection<Module> Modules(const CatalogFilter& filter = nullptr) const
    {
        return BuildCollection<Module>(_parts, "Module", filter);
    }

private:
    QDomElement _parts;
    QDomElement _assemblies;

    template<typename ProbePart>
    PartCollection<ProbePart> BuildCollection(const QDomElement& source, const QString& tagName, const CatalogFilter& filter = nullptr) const
    {
        const auto xmlElements = source.elementsByTagName(tagName);

        PartCollection<ProbePart> result;

        for(int i = 0; i < xmlElements.count(); ++i)
        {
            const auto elem = xmlElements.item(i);
            if(elem.isElement())
            {
                const auto domElem = elem.toElement();
                if(filter)
                {
                    if (filter(domElem))
                        result.push_back(ProbePart(domElem));
                }
                else
                    result.push_back(ProbePart(domElem));
            }
        }

        return result;
    }
};

struct RotaryAxis
{
    double X;
    double Y;
    double Z;

    double I;
    double J;
    double K;
};

struct RotaryControlInfo
{
    RotationAddress::Address Address;
    double Min;
    double Max;
};

class MachinePartRotaryAxis
{
public:
    MachinePartRotaryAxis() = default;

    explicit MachinePartRotaryAxis(const QDomElement& xmlDom)
    {
        const auto controlInfoTag = xmlDom.firstChildElement("control_info");
        const auto rotaryInfoTag = xmlDom.firstChildElement("simple_rotary");

        _controlInfo.Address = RotationAddress::FromString(controlInfoTag.attribute("ADDRESS"));
        _controlInfo.Min = controlInfoTag.attribute("MIN", "0").toDouble();
        _controlInfo.Max = controlInfoTag.attribute("MAX", "0").toDouble();

        _axis.X = rotaryInfoTag.attribute("X").toDouble();
        _axis.Y = rotaryInfoTag.attribute("Y").toDouble();
        _axis.Z = rotaryInfoTag.attribute("Z").toDouble();
        _axis.I = rotaryInfoTag.attribute("I").toDouble();
        _axis.J = rotaryInfoTag.attribute("J").toDouble();
        _axis.K = rotaryInfoTag.attribute("K").toDouble();
    }

    RotaryControlInfo ControlInfo() const noexcept
    {
        return _controlInfo;
    }

    RotaryAxis Axis() const noexcept
    {
        return _axis;
    }

private:
    RotaryControlInfo _controlInfo;
    RotaryAxis _axis;
};

struct Rgb
{
    byte R;
    byte G;
    byte B;
};

class DmtFileModel
{
public:
    explicit DmtFileModel(const QDomElement& xml)
    {
        const auto meshFile = xml.firstChildElement("path").attribute("FILE");
        const auto realMeshFile = ResolveRealObjectFilePath(meshFile.toStdString());

        if(!realMeshFile.has_value())
            throw std::exception("Dmt file model can not be loaded");

        _actualMeshFile = QString::fromStdString(realMeshFile.value().string());

        const auto color = xml.firstChildElement("rgb");

        _color.R = static_cast<byte>(color.attribute("R").toInt());
        _color.G = static_cast<byte>(color.attribute("G").toInt());
        _color.B = static_cast<byte>(color.attribute("B").toInt());
    }

    QString MeshFile() const noexcept
    {
        return _actualMeshFile;
    }

    Rgb Color() const noexcept
    {
        return _color;
    }

    RgbaF ColorF() const noexcept
    {
        return RgbaF(static_cast<double>(_color.R) / 255, static_cast<double>(_color.G) / 255, static_cast<double>(_color.B) / 255);
    }

private:
    QString _actualMeshFile;
    Rgb _color;

    static std::optional<std::filesystem::path> ResolveRealObjectFilePath(const std::string& declaredDmtPath, int stepBack = 1)
    {
        const auto fileName = std::filesystem::path(declaredDmtPath).filename().replace_extension(".obj").string();
        std::filesystem::path currentPath = std::filesystem::current_path();

        for(int i = 0; i < stepBack; ++i) {
            if(currentPath.has_parent_path()) {
                currentPath = currentPath.parent_path();
            } else {
                break;
            }
        }

        for(const auto& entry : std::filesystem::recursive_directory_iterator(currentPath)) {
            if(entry.path().filename() == fileName) {
                return relative(entry.path(), std::filesystem::current_path());
            }
        }

        return std::nullopt;
    }
};

class MachineModelList
{
public:
    MachineModelList() = default;
    explicit MachineModelList(const QDomElement& xml)
    {
        const auto dmts = xml.elementsByTagName("dmt_file");

        for (int i = 0; i < dmts.count(); i++)
        {
            const auto elem = dmts.item(i).toElement();

            _parts.emplace_back(elem);
        }
    }

    std::vector<DmtFileModel> Parts() const
    {
        return _parts;
    }

private:
    std::vector<DmtFileModel> _parts;
};

class MachinePart
{
public:
    explicit MachinePart(const QDomElement& xml)
    {
        const auto nestedParts = xml.childNodes();
        for (int i = 0; i < nestedParts.count(); i++)
        {
            const auto elem = nestedParts.item(i).toElement();

            if (elem.tagName() == "axis")
            {
                _has_rotation_axis = true;
                _axes = MachinePartRotaryAxis(elem);
            }

            else if (elem.tagName() == "model_list")
            {
                _models = MachineModelList(elem);
            }

            else if (elem.tagName() == "machine_part")
            {
                // Recursion. Looks like quite nice solution for not so big XML, but need to invent some better way to to that for big XML's. Also hope MTD files still so small through
                _nestedMachinePart.push_back(new MachinePart(elem));
            }
        }
    }

    bool CanRotate() const noexcept
    {
        return _has_rotation_axis;
    }

    RotAddress RotationAxisAddress() const noexcept
    {
        return _axes.ControlInfo().Address;
    }

    MachineModelList Models() const noexcept
    {
        return _models;
    }

    MachinePartRotaryAxis Axis() const noexcept
    {
        return _axes;
    }

    std::vector<MachinePart*> ChildMachines() const noexcept
    {
        return _nestedMachinePart;
    }

private:
    bool _has_rotation_axis = false;

    MachineModelList _models;
    MachinePartRotaryAxis _axes;
    std::vector<MachinePart*> _nestedMachinePart{};
};

using MachinePartCollection = std::vector<MachinePart>;

class ProbeHeadLoader
{
public:
    static MachinePartCollection FromMtd(const QString& path) {
        QDomDocument document;
        QFile file(path);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Failed to open the file for reading.";
        }

        if(!document.setContent(&file)) {
            qDebug() << "Failed to parse the file into a DOM tree.";
            file.close();
        }
        file.close();

        const auto xmlParts = document.firstChildElement("machine").childNodes();

        MachinePartCollection result;

        for(int i = 0; i < xmlParts.count(); i++)
        {
            const auto elem = xmlParts.item(i);
            if(!elem.hasChildNodes()) // empty <machine_part/>
                continue;
            result.emplace_back(elem.toElement());
        }

        return result;
    }
};
