#pragma once

#include "pch.h"
#include "EigenUtils.hpp"
#include "LineData.hpp"
#include "Point3D.h"

class PolyLine;

using vtkPolyDataPointer = vtkSmartPointer<vtkPolyData>;
using PolyLineCallback = std::function<void(PolyLine*)>;

class PolyLine
{
    template<typename TLineData>
    using is_line_data = std::disjunction<std::is_same<TLineData, LineData>, std::is_same<TLineData, CLineData>>;

    template<typename... TLineData>
    using is_line_data_struct = std::conjunction<is_line_data<TLineData>...>;

public:
    using PolyLineCallbackIdType = unsigned long long;

    PolyLine()
    {
        _points = vtkSmartPointer<vtkPoints>::New();
        _lines = vtkSmartPointer<vtkCellArray>::New();
        _colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        _colors->SetNumberOfComponents(3); 
        _polyData = vtkPolyDataPointer::New();

        _includeMarkerManager = false;
    }

    ~PolyLine() = default;

    // Some day i'll invent a normal way to copy and move this object, but for this moment - NO
    PolyLine(const PolyLine&) = delete;
    PolyLine(PolyLine&&) = delete;

    PolyLine& operator=(const PolyLine&) = delete;
    PolyLine& operator=(PolyLine&&) = delete;

    template<typename ...TLineData,
        std::enable_if_t<is_line_data_struct<TLineData...>::value, bool> = true>
    void InsertMany(const TLineData&... lines)
    {
        InsertLine(lines...);
    }

    void InsertLine(const CLineData& line)
    {
        InsertLine(line.Begin, line.End, line.Color.ToRGBDoubleArray().data());
    }

    void InsertLine(const LineData& line)
    {
        InsertLine(line.Begin, line.End, line.Color.ToEigenVector3d());
    }

    void InsertLine(const double* begin, const double* end, const double* color)
    {
        _points->InsertNextPoint(begin);
        _points->InsertNextPoint(end);
        _polyData->SetPoints(_points);

        const vtkNew<vtkPolyLine> line {};

        line->GetPointIds()->SetNumberOfIds(2);
        line->GetPointIds()->SetId(0, _lineInsertId);
        line->GetPointIds()->SetId(1, ++_lineInsertId);

        _lineInsertId++;
        _lines->InsertNextCell(line);
        _polyData->SetLines(_lines);

        _colors->InsertNextTuple(NormalizeColor(color));

        _polyData->GetCellData()->SetScalars(_colors);

        UpdateData();
        NotifyEvent();

        _insertsCount++;

        InsertNewLinePosition(Eigen::Vector3d(begin[0], begin[1], begin[2]));
        InsertNewLinePosition(Eigen::Vector3d(end[0], end[1], end[2]));
    }

    void InsertLine(const Eigen::Vector3d& begin, const Eigen::Vector3d& end, const Eigen::Vector3d& color = {0, 1, 0}) noexcept
    {
        InsertLine(EigenVector3dToCArray(begin), EigenVector3dToCArray(end), EigenVector3dToCArray(color));
    }

    void ColorLine(const vtkIdType lineIdx, const Eigen::Vector3d& color) noexcept
    {
        if(lineIdx > _colors->GetNumberOfTuples() - 1 || lineIdx < 0)
            return;
        const auto normColor = NormalizeColor(EigenVector3dToCArray(color));
        _colors->SetTuple(lineIdx, normColor);
        UpdateData();
        NotifyEvent();
    }

    void ColorMany(const std::vector<vtkIdType>& indices, const Eigen::Vector3d& color)
    {
        for(const auto& idx : indices)
            ColorLine(idx, color);
    }

    vtkIdType GetInsertsCount() const noexcept
    {
        return _insertsCount; 
    }

    vtkPolyDataPointer& GetData()
    {
        return _polyData;
    }

    vtkSmartPointer<vtkPoints> GetVtkPoints()
    {
        return _points;
    }

    std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d>> GetLineNodes()
    {
        return _pointsPositions;
    }

    vtkSmartPointer<vtkActor> GetActor() noexcept
    {
        UpdateData();
        if (!_ownActor || _ownActor.GetPointer() == nullptr)
        {
            const auto actor = vtkSmartPointer<vtkActor>::New();
            _ownActor = actor;
        }

        _ownActor->SetMapper(_ownMapper);
        _ownActor->Modified();
        return _ownActor;
    }

    PolyLineCallbackIdType SetCallback(const PolyLineCallback& cb) noexcept
    {
        static PolyLineCallbackIdType idCounter = 0;

        _callbackListeners[idCounter] = cb;
        return idCounter++;
    }

    void RemoveCallback(PolyLineCallbackIdType id) noexcept
    {
        const auto it = _callbackListeners.find(id);

        if(it != _callbackListeners.end())
            _callbackListeners.erase(it);
    }

protected:
    void NotifyEvent()
    {
        for(const auto& [_, fn] : _callbackListeners)
            fn(this);
    }

private:
    vtkSmartPointer<vtkPoints> _points;
    vtkSmartPointer<vtkCellArray> _lines;
    vtkSmartPointer<vtkUnsignedCharArray> _colors;
    vtkPolyDataPointer _polyData;
    vtkSmartPointer<vtkActor> _ownActor;
    vtkSmartPointer<vtkPolyDataMapper> _ownMapper;

    vtkIdType _lineInsertId {};
    vtkIdType _insertsCount {};

    bool _includeMarkerManager;
    MarkerManager* _makerManager = nullptr;

    std::unordered_map<PolyLineCallbackIdType, PolyLineCallback> _callbackListeners;

    std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d>> _pointsPositions {};

    void UpdateData() noexcept
    {
        //TODO: Fix re-creating mapper every time when line appends or add methods for adding multiple lines in one operation
        _ownMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        _ownMapper->SetScalarModeToUseCellData();
        _ownMapper->SetInputData(_polyData);
        _ownMapper->Update();

        if (_ownActor && _ownActor.GetPointer())
        {
            _ownActor->SetMapper(_ownMapper);
            _ownActor->Modified();
        }
    }

    void InsertNewLinePosition(const Eigen::Vector3d& point)
    {
        bool exists = false;
        for(const auto& p : _pointsPositions) {
            if(p.isApprox(point)) {
                exists = true;
                break;
            }
        }

        if(!exists) {
            _pointsPositions.push_back(point);
        }
    }

    // Map given array values to range [0, 255] as double
    static double* NormalizeColor(const double* vec)
    {
        auto* rgb = new double[3];

        for(int i = 0; i < 3; ++i) {
            rgb[i] = (std::round(std::clamp(vec[i], 0.0, 1.0) * 255));
        }

        return rgb;
    }
};