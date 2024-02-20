#pragma once
#include "MarkerManager.hpp"
#include "PolyLine.hpp"

class SceneWidget : public QVTKOpenGLNativeWidget {
    Q_OBJECT
    friend class MeasurementWindow;

public:
    explicit SceneWidget(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    ~SceneWidget() override
    {
        renderWindow()->RemoveAllObservers();
        delete _renderTimer;
        delete _polyLine;
    }

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

    QTimer *_renderTimer;

    vtkSmartPointer<vtkRenderer> _renderer;

    //task
    PolyLine* _polyLine;
    MarkerManager* _markerManager;

    std::vector<vtkSmartPointer<vtkActor>> _demoSpheres;
};
