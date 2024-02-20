#include "pch.h"

#include "measurementwindow.h"

#include "CreateLineDialog.h"
#include "CreateSphereDialog.h"
#include "vtkUtils.hpp"
#include "generators.hpp"

MeasurementWindow::MeasurementWindow(QWidget *parent) : QMainWindow(parent), _ui(new Ui::MeasurementWindow) {
    _ui->setupUi(this);

    _renderer = _ui->sceneWidget->getRenderer();

    connect(_ui->pushButton, &QPushButton::clicked, _ui->sceneWidget, &SceneWidget::clear);
    connect(_ui->pushButton, &QPushButton::clicked, _ui->sceneWidget, &SceneWidget::requestRender);

    connect(_ui->addSphereButton, &QPushButton::clicked, this, &MeasurementWindow::addSphere);
    connect(_ui->addLineButton, &QPushButton::clicked, this, &MeasurementWindow::addLine);

    connect(_ui->paintSomehowButton, &QPushButton::clicked, this, &MeasurementWindow::demoPaintObject);

    connect(_ui->markupPolyLineButton, &QPushButton::clicked, this, &MeasurementWindow::demoSetMarkersOnPolyLine);

    connect(_ui->resetCameraButton, &QPushButton::clicked, this, [this]() {
        _ui->sceneWidget->_renderer->ResetCamera();
        _ui->sceneWidget->renderScene();
    });
}

MeasurementWindow::~MeasurementWindow() {
    delete _ui;
}

void MeasurementWindow::addLine()
{
    const auto lineDialog = new CreateLineDialog(this);

    lineDialog->exec();

    const auto lineData = lineDialog->Data();

    _ui->sceneWidget->_polyLine->InsertLine(lineData.Begin, lineData.End, lineData.Color.ToEigenVector3d());
}

void MeasurementWindow::addSphere()
{
    const auto sphereDialog = new CreateSphereDialog(this);

    sphereDialog->exec();

    const auto sphereData = sphereDialog->Data();
    const auto sphere = CreateSphere(sphereData);
    AddActorsToRenderer(_renderer, sphere);

    _ui->sceneWidget->_demoSpheres.push_back(sphere);
}

void MeasurementWindow::paintObject()
{
    
}

void MeasurementWindow::demoPaintObject()
{
    const auto color = QColorDialog::getColor(Qt::white, this, "Please, provide color for demo");

    if(color.isValid()) {
        const auto rgbafColor = RgbaF::FromQColor(color);

        std::vector<vtkIdType> indices;

        for(vtkIdType i = 1; i < _ui->sceneWidget->_polyLine->GetInsertsCount(); i += 2)
            indices.push_back(i);

        _ui->sceneWidget->_polyLine->ColorMany(indices, rgbafColor.ToEigenVector3d());

        for (const auto& it: _ui->sceneWidget->_demoSpheres)
        {
            it->GetProperty()->SetColor(rgbafColor.ToRGBDoubleArray().data());
        }

        _ui->sceneWidget->renderScene();
    }
}

void MeasurementWindow::demoSetMarkersOnPolyLine()
{
    auto points = _ui->sceneWidget->_polyLine->GetLineNodes();

    if(points.empty())
        return;

    const auto beginMarker = _ui->sceneWidget->_markerManager->CreateMarkerOfType(StdMarkers::Circle, points.at(0)).value();

    points.erase(points.begin());

    const auto nodeMarkers = _ui->sceneWidget->_markerManager->CreateMany(StdMarkers::Path, points).value();

    AddActorsToRenderer(_ui->sceneWidget->_renderer, beginMarker);

    AddActorsToRenderer(_ui->sceneWidget->_renderer, nodeMarkers);
    
    _ui->sceneWidget->renderScene();
    _ui->sceneWidget->_markerManager->Update();
}
