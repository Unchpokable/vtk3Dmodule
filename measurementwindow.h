#pragma once

#include "ui_measurementwindow.h"

namespace Ui {
    class MeasurementWindow;
};

class MeasurementWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MeasurementWindow(QWidget *parent = nullptr);
    ~MeasurementWindow() override;

private slots:
    void addSphere();
    void addLine();
    void makeScreenshot();
    void rotateMachineHead();
    void demoSetMarkersOnPolyLine();

private:
    Ui::MeasurementWindow *_ui;

    PolyLine* _lineContainer;

    vtkSmartPointer<vtkRenderer> _renderer;
};
