#include "pch.h"

#include "scenewidget.h"
#include "ViewCapture.h"
#include "MachineHead.hpp"
#include "vtkUtils.hpp"
#include "MarkerManager.hpp"
#include "ProbeToolAssembly.hpp"

SceneWidget::SceneWidget(QWidget *parent, Qt::WindowFlags flags) : QVTKOpenGLNativeWidget(parent) {
    _renderer = vtkSmartPointer<vtkRenderer>::New();
    _renderer->GetActiveCamera()->ParallelProjectionOn();
    _renderer->GradientBackgroundOn();
    _renderer->SetBackground(0.3, 0.6, 0.6);
    _renderer->SetBackground2(.7, .7, .7);

    _renderer->AddObserver(vtkCommand::StartEvent, this, &SceneWidget::prepareToRender);

    vtkRenderWindow *window = renderWindow();
    window->AddRenderer(_renderer);
    window->SetPointSmoothing(true);
    window->SetLineSmoothing(true);
    window->SetPolygonSmoothing(true);

    _markerManager = new MarkerManager(_renderer->GetActiveCamera(), renderWindow());

    _renderTimer = new QTimer(this);
    _renderTimer->setInterval(100);
    //_renderTimer->start();
    //_renderTimer->setSingleShot(true);

    connect(_renderTimer, &QTimer::timeout, this, &SceneWidget::renderScene);

    // -------------------------

    ProbePartCatalog catalog("p/ProbePartCatalogue.xml");

    ProbeToolAssembly manualTool;
    manualTool.Build(catalog, {"TP20_EM2", "M2x90_CF", "M2_20x1_TC"});

    const auto manualToolModel = manualTool.Weld();

    SetActorLightingPlastic(manualToolModel);

    /*_renderer->AddActor(manualToolModel);*/

    _machineHead = new MachineHead(ProbeHeadLoader::FromMtd("ph/PH10M/PH10M.xml"));
    _machineHead->AddStylus(manualTool);

    _machineHead->RotateRZ(-90, { 1, 0, 0 });
    const auto actors = _machineHead->Actors();

    //AddActorsToRenderer(_renderer, actors);

    //auto mountTransform = vtkSmartPointer<vtkTransform>::New(); // rz
    //mountTransform->RotateX(30);

    //actors[0]->SetUserTransform(mountTransform);

    //auto headBTransform = vtkSmartPointer<vtkTransform>::New();
    //headBTransform->Concatenate(mountTransform);
    //headBTransform->RotateZ(45);

    //actors[1]->SetUserTransform(headBTransform);

    //auto headATransform = vtkSmartPointer<vtkTransform>::New();
    //headATransform->Concatenate(headBTransform);
    //headATransform->RotateX(60);

    //actors[2]->SetUserTransform(headATransform);

    auto table = new QTableWidget(3, 3);
    table->setWindowTitle("VTK-Qt Texture Example");
    const QStringList labels { "Column 1", "Column 2", "Column 3" };
    table->setHorizontalHeaderLabels(labels);

    for(int i = 0; i < 3; ++i) 
        for(int j = 0; j < 3; ++j) 
        {
            table->setItem(i, j, new QTableWidgetItem(QString("Data %1-%2").arg(i + 1).arg(j + 1)));
        }

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    /*const auto tableAsActor = CreateTextureFromQWidget(&table);

    _renderer->AddViewProp(tableAsActor);*/

    auto widgetRepresentation = vtkSmartPointer<vtkQWidgetRepresentation>::New();
    widgetRepresentation->SetWidget(table);

    _renderer->AddViewProp(widgetRepresentation);

    _renderer->ResetCamera();
    showGizmo();
}

std::string SceneWidget::demoScreenshot()
{
    ViewCapture cap(_renderer);

    auto rawConfig = ParallelCameraSettings::FromCamera(_renderer->GetActiveCamera()).AroundFocal(120, 40);

    cap.SetSpecialCameraConfigs(rawConfig);
    auto size = renderWindow()->GetSize();

    Size s{ size[0], size[1] };

    const auto oldRots = _machineHead->Rotations();

    const auto path = cap.TakeScreenshot(s, "AwesomeScreenshot4.png", [this](const vtkRenderer*) {
        _machineHead->SetZero();
        _machineHead->RotatePart(RotAddress::A, 100);
        _machineHead->RotatePart(RotAddress::B, 100);
    }, true);

    _machineHead->RotationsFromMap(oldRots);

    return path;
}

vtkCamera* SceneWidget::camera() const {
    return _renderer->GetActiveCamera();
}

void SceneWidget::requestRender() const
{
    _renderTimer->start();
}

void SceneWidget::clear() const
{
    const auto actorCollection = _renderer->GetActors();

    auto lastActor = actorCollection->GetLastActor();
    if(lastActor) {
        _renderer->RemoveActor(lastActor);
    }
}

void SceneWidget::renderScene()
{
    //static double angle = 1;
    //_machineHead->RotatePart(RotAddress::A, angle);
    //_machineHead->RotatePart(RotAddress::B, angle++);

    //_machineHead->RemoveStylus();
    _renderer->GetRenderWindow()->Render();
}

void SceneWidget::prepareToRender(vtkObject *caller, ulong eventId, void *callData) {
    // empty
}

const vtkSmartPointer<vtkRenderer>& SceneWidget::getRenderer()
{
    return _renderer;
}

void SceneWidget::updateRendererCamera(vtkObject*, ulong, void*)
{
    _renderer->GetActiveCamera()->Modified();
}

void SceneWidget::showGizmo()
{
    vtkNew<vtkAxesActor> axes;
    axes->SetShaftTypeToCylinder();
    axes->SetXAxisLabelText("X");
    axes->SetYAxisLabelText("Y");
    axes->SetZAxisLabelText("Z");
    axes->SetTotalLength(1.0, 1.0, 1.0);
    axes->SetCylinderRadius(0.5 * axes->GetCylinderRadius());
    axes->SetConeRadius(1.025 * axes->GetConeRadius());
    axes->SetSphereRadius(1.5 * axes->GetSphereRadius());

    _gizmo = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    _gizmo->SetOrientationMarker(axes);
    _gizmo->SetInteractor(renderWindow()->GetInteractor());
    _gizmo->SetViewport(.8, .8, 1, 1);
    _gizmo->EnabledOn();
    _gizmo->InteractiveOn();
}
