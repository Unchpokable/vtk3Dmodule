#pragma once
#include "MarkerManager.hpp"
#include "MachineHead.hpp"
#include "PolyLine.hpp"
#include "pch.h"

class SceneWidget : public QVTKOpenGLNativeWidget {
    Q_OBJECT
    friend class MeasurementWindow;

public:
    explicit SceneWidget(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    ~SceneWidget() override
    {
        renderWindow()->RemoveAllObservers();
        delete _renderTimer;
        //delete _polyLine;
    }

    std::string demoScreenshot();

    vtkCamera* camera() const;

    const vtkSmartPointer<vtkRenderer>& getRenderer();

public slots:
    void requestRender() const;

    void clear() const;

private slots:
    void renderScene();

private:
    void prepareToRender(vtkObject *caller, ulong eventId, void *callData);

    void updateRendererCamera(vtkObject*, ulong, void*);

    void showGizmo();

    QTimer *_renderTimer;

    vtkSmartPointer<vtkRenderer> _renderer;

    //task
    PolyLine* _polyLine;
    MarkerManager* _markerManager;
    vtkSmartPointer<vtkOrientationMarkerWidget> _gizmo;
    MachineHead* _machineHead;
    std::vector<vtkSmartPointer<vtkActor>> _demoSpheres;
};
